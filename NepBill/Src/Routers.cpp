#include "PCH.h"
#include <string_view>
#include "NepalDateConverter.h"
#include "Maths.h"
#include "NepBill.h"
#include "Routers.h"
#include "jwt-cpp/jwt.h"
#include "tl/expected.hpp"
#include "sodium.h"

static constexpr uint32_t kSaltSize = crypto_pwhash_SALTBYTES; // 16 bytes usually

const std::string JWT_COOKIE_KEY = "69f2e46b088b2b73d2a71d87cfb8813a34a819b16892e6205cfbf03738e4df45";
constexpr int MaxLoginCookieAge = 60 * 60 * 24 * 30; // 30 days

namespace NepBill
{
    bool IsAccountIdEmail(std::string Id)
    {
        if (Id.find("@") != std::string::npos && Id.find(".com") != std::string::npos)
        {
            return true;
        }

        return false;
    }
    /**
     * @brief Parses an HTML date/datetime string from Crow JSON into a std::time_t.
     * Handles formats: "YYYY-MM-DDTHH:MM:SS", "YYYY-MM-DD HH:MM:SS", or "YYYY-MM-DD".
     *
     * @param json_obj The crow::json::rvalue containing your data.
     * @param key The key string (e.g., "createdAt").
     * @return std::time_t The parsed UNIX timestamp.
     */
    std::time_t ParseHtmlDateTime(const crow::json::rvalue &json_obj, const std::string &key)
    {
        // 1. Safety check: Check if the key exists
        if (!json_obj.has(key))
        {
            throw std::invalid_argument("Key '" + key + "' missing from JSON.");
        }

        // 2. Extract raw string
        std::string raw_date = json_obj[key].s();

        if (raw_date.empty())
        {
            throw std::invalid_argument("Date field is empty.");
        }

        // 3. Initialize time structure
        std::tm t = {};
        t.tm_isdst = -1; // Let the system determine Daylight Saving Time

        std::istringstream ss(raw_date);

        // 4. Try parsing "YYYY-MM-DDTHH:MM:SS" (HTML <input type="datetime-local"> format)
        ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

        if (ss.fail())
        {
            // Fallback 1: Try space separator "YYYY-MM-DD HH:MM:SS" (SQL timestamp format)
            ss.clear();
            ss.str(raw_date);
            ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
        }

        if (ss.fail())
        {
            // Fallback 2: Try date-only "YYYY-MM-DD" and set time to midnight
            ss.clear();
            ss.str(raw_date);
            ss >> std::get_time(&t, "%Y-%m-%d");
            if (!ss.fail())
            {
                t.tm_hour = 0;
                t.tm_min = 0;
                t.tm_sec = 0;
            }
        }

        if (ss.fail())
        {
            throw std::runtime_error("Failed to parse date string format: " + raw_date);
        }

        // 5. Convert to timestamp
        std::time_t timestamp = std::mktime(&t);
        if (timestamp == -1)
        {
            throw std::runtime_error("Invalid calendar date values provided.");
        }

        return timestamp;
    }

    /**
     * @brief Converts a standard C++ timestamp into a string formatted as "YYYY-MM-DD HH:MM:SS"
     *
     * @param timestamp The time_t value to convert.
     * @param use_iso_t Set to true for HTML datetime-local format ("YYYY-MM-DDTHH:MM:SS")
     * @return std::string Formatted date-time string
     */
    std::string FormatDateForHtml(std::time_t timestamp, bool use_iso_t = false)
    {
        std::tm *t = std::localtime(&timestamp);
        if (!t)
        {
            return "";
        }

        std::ostringstream oss;
        const char *format = use_iso_t ? "%Y-%m-%dT%H:%M:%S" : "%Y-%m-%d %H:%M:%S";
        oss << std::put_time(t, format);
        return oss.str();
    }

    /**
     * @brief Converts a std::tm structure into a string formatted as "YYYY-MM-DD HH:MM:SS"
     *
     * @param t Reference to std::tm structure.
     * @param use_iso_t Set to true for HTML datetime-local format ("YYYY-MM-DDTHH:MM:SS")
     * @return std::string Formatted date-time string
     */
    std::string FormatDateForHtml(const struct tm &t, bool use_iso_t = false)
    {
        std::ostringstream oss;
        const char *format = use_iso_t ? "%Y-%m-%dT%H:%M:%S" : "%Y-%m-%d %H:%M:%S";
        oss << std::put_time(&t, format);
        return oss.str();
    }

    crow::response LoggedIn(NepBill::App &Backend, const crow::request &Req)
    {
        crow::json::wvalue Result;
        crow::response res;
        // 1. Extract the Cookie header
        std::string cookies = Req.get_header_value("Cookie");

        // Simple parsing to find "auth_token="
        // (In production, use a proper HTTP parser)
        size_t pos = cookies.find("auth_token=");
        if (pos == std::string::npos)
        {
            Result["LoggedIn"] = false;
            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 200;

            return res;
        }

        std::string token = cookies.substr(pos + 11);
        size_t end = token.find(";");
        if (end != std::string::npos)
            token = token.substr(0, end);

        try
        {
            // 2. Verify the JWT
            auto verifier = jwt::verify()
                                .allow_algorithm(jwt::algorithm::hs256{JWT_COOKIE_KEY})
                                .with_issuer("NepBill");

            auto decoded = jwt::decode(token);
            verifier.verify(decoded);
            if (decoded.has_payload_claim("Accountuuid"))
            {
                std::cout << "Accountuuid: " << decoded.get_payload_claim("Accountuuid").as_string() << "\n";
                Result["AccountId"] = decoded.get_payload_claim("Accountuuid").as_string();
            }
            Result["LoggedIn"] = true;

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 200;

            return res;
        }
        catch (...)
        {
        }
    }

    crow::response AccountQueryInfo(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
            return crow::response(400);

        crow::json::wvalue Result;
        crow::response res;

        AccountQuery AccountQuery;
        std::string AccountId = Json["AccountId"].s();
        AccountQuery.UniqueId = UUID::FromString(std::string_view(AccountId.c_str()));
        auto Accounts = GetAccounts(Backend.Database, AccountQuery);

        if (Accounts.size() == 0)
        {
            Result["Return"] = false;
            Result["Message"] = "AccountId Not Found!";
            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 200;

            return res;
        }

        auto& Account = Accounts[0];

        std::cout << "Query Account Info AccountId: " << AccountId << "\n";
        Result["Return"] = true;
        Result["Roles"] = AccountRolesToStr(Account.Role);

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AccountLogout(const crow::request &Req)
    {
        crow::response res(200);

        // Set cookie to expire, effectively clearing it from the browser
        res.add_header("Set-Cookie",
                       // Cookie format: name=value; HttpOnly; Secure; SameSite=Strict; Max-Age=3600
                       "auth_token=; HttpOnly; Path=/; Secure; SameSite=Strict; Max-Age=0");

        crow::json::wvalue json;
        json["Success"] = true;

        res.set_header("Content-Type", "application/json");
        res.write(json.dump());
        return res;
    }

    crow::response IndexDeafult(NepBill::App &Backend, const crow::request &Req)
    {
        std::ifstream file("templates/index.html");

        if (!file)
            return crow::response(404, "Could not open file");
        std::stringstream ss;
        ss << file.rdbuf();

        auto content = ss.str();

        std::cout << "Content size = "
                  << content.size()
                  << std::endl;

        return crow::response(200, content);
    }

    crow::response ApiLogin(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
            return crow::response(400);

        crow::json::wvalue Result;
        crow::response res;

        std::string AccountId =
            Json["AccountId"].s();
        std::string Password =
            Json["Password"].s();

        bool IsEmail = IsAccountIdEmail(AccountId);

        ContactFormQuery AccountRegisterInfoQuery;

        if (IsEmail)
            AccountRegisterInfoQuery.Email = AccountId;
        else
            AccountRegisterInfoQuery.PhoneNumber = AccountId;
        AccountRegisterInfoQuery.Type = ContactSubjectType::RegisterAccount;

        auto RegisterContactInfo = GetContactForms(Backend.Database, AccountRegisterInfoQuery);

        std::cout << RegisterContactInfo.size() << " Found this many register contact infos" << "\n";
        if (RegisterContactInfo.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "AccountId Not Found";
        }
        else
        {
            AccountQuery AccountQuery;
            AccountQuery.ContactFormID = RegisterContactInfo[0].UniqueId;

            auto AccountInfo = GetAccounts(Backend.Database, AccountQuery);
            std::cout << AccountInfo.size() << " Found this many account infos" << "\n";

            if (AccountInfo.size() == 0)
            {
                Result["State"] = false;
                Result["Message"] = "No Account info related to this AccountId Found!";
            }
            else
            {
                auto &Account = AccountInfo[0];

                EncryptionManager EncryptionManager;
                for (auto &i : Account.PasswordHash)
                    std::cout << i;
                std::cout << "\n";
                std::cout << Password << "\n";

                if (EncryptionManager.verifyPassword(Password, Account.PasswordHash) == false)
                {
                    std::cout << "Invalid password!\n";
                    Result["State"] = false;
                    Result["Message"] = "Invalid credentials!";
                }
                else
                {
                    std::cout << "VALID ENTRY LOGIN" << "\n";
                    // Generate JWT
                    auto token = std::string(jwt::create()
                                                 .set_issuer("NepBill")
                                                 .set_type("JWS")
                                                 .set_payload_claim("Accountuuid", jwt::claim(std::string(Account.UniqueId.ToString())))
                                                 .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{MaxLoginCookieAge}) // 1 hour
                                                 .sign(jwt::algorithm::hs256{JWT_COOKIE_KEY}));

                    // Set token in HttpOnly Cookie
                    // Cookie format: name=value; HttpOnly; Secure; SameSite=Strict; Max-Age=3600
                    std::string cookie_header = std::string("auth_token=") + token +
                                                "; HttpOnly; Path=/; Secure; SameSite=Strict; Max-Age=" + std::to_string(MaxLoginCookieAge);
                    res.add_header("Set-Cookie", cookie_header);

                    Result["State"] = true;
                    Result["Message"] = "Login successful";
                }
            }
        }

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    std::string GetFormDataPartValue(const crow::multipart::message &Message, const std::string &key)
    {
        for (const auto &part : Message.parts)
        {
            auto it = part.headers.find("Content-Disposition");
            if (it != part.headers.end())
            {
                // Crow stores parameters (like name="..." or filename="...") inside params map
                const auto &params = it->second.params;
                auto nameIt = params.find("name");

                if (nameIt != params.end() && nameIt->second == key)
                {
                    return part.body; // Returns field value or binary file content
                }
            }
        }
        return "";
    }

    crow::response SubmitContactForm(NepBill::App &Backend, const crow::request &Req)
    {
        crow::json::wvalue Result;
        crow::response res;
        // 1. Parse the request body as a multipart message
        crow::multipart::message Message(Req);

        // Dynamic storage variables
        std::string Phone, Email, SubjectTypeStr, ContactMessage;
        ContactSubjectType SubjectType;

        SubjectTypeStr = GetFormDataPartValue(Message, "issue_type");
        // Usage:
        Phone = GetFormDataPartValue(Message, "phone");
        Email = GetFormDataPartValue(Message, "email");
        ContactMessage = GetFormDataPartValue(Message, "message");
        std::cout << "SubjectTypeStr: " << SubjectTypeStr << "\n";
        SubjectType = StrToContactSubjectType(SubjectTypeStr.c_str()).value();

        ContactFormInfo ContactForm;
        memcpy(ContactForm.PhoneNumber.data(), Phone.data(), Phone.size());
        memcpy(ContactForm.Email.data(), Email.data(), Email.size());
        memcpy(ContactForm.Message.data(), ContactMessage.data(), ContactMessage.size());
        ContactForm.Type = SubjectType;
        ContactForm.State = ContactStates::Processing;
        Insert(Backend.Database, ContactForm);

        switch (SubjectType)
        {
        case ContactSubjectType::RegisterAccount:
        {
            auto BusinessName = GetFormDataPartValue(Message, "business_name");
            auto OwnerName = GetFormDataPartValue(Message, "owner_name");
            auto PanNumber = GetFormDataPartValue(Message, "pan_number");
            auto VatNumber = GetFormDataPartValue(Message, "vat_number");
            auto BusinessContinent = GetFormDataPartValue(Message, "business_continent");
            auto BusinessCountry = GetFormDataPartValue(Message, "business_country");
            auto BusinessAddress = GetFormDataPartValue(Message, "business_address");
            auto AccPassword = GetFormDataPartValue(Message, "account_password");

            auto PanContent = GetFormDataPartValue(Message, "pan_vat_photo");
            auto BusinessCertificationContent = GetFormDataPartValue(Message, "business_cert_photo");
            auto BusinessFrontPhotoContent = GetFormDataPartValue(Message, "business_front_photo");

            PdfDocument PanDocument, BusinessCertificateDocument;
            Image BusinessFrontPhoto;

            memcpy(BusinessFrontPhoto.Name.data(), BusinessName.c_str(), BusinessName.size());
            BusinessFrontPhoto.Data.resize(BusinessFrontPhotoContent.size());
            BusinessFrontPhoto.Data.shrink_to_fit();
            memcpy(BusinessFrontPhoto.Data.data(), BusinessFrontPhotoContent.data(), BusinessFrontPhotoContent.size());

            memcpy(BusinessCertificateDocument.Name.data(), BusinessName.c_str(), BusinessName.size());
            BusinessCertificateDocument.Data.resize(BusinessCertificationContent.size());
            BusinessCertificateDocument.Data.shrink_to_fit();
            memcpy(BusinessCertificateDocument.Data.data(), BusinessCertificationContent.data(), BusinessCertificationContent.size());

            memcpy(PanDocument.Name.data(), BusinessName.c_str(), BusinessName.size());
            PanDocument.Data.resize(PanContent.size());
            PanDocument.Data.shrink_to_fit();
            memcpy(PanDocument.Data.data(), PanContent.data(), PanContent.size());

            RegisterAccountContactFormInfo RegisterInfo;

            RegisterInfo.ContactID = ContactForm.UniqueId;
            memcpy(RegisterInfo.Name.data(), BusinessName.data(), BusinessName.size());
            memcpy(RegisterInfo.PrimaryPhoneNumber.data(), Phone.data(), Phone.size());
            memcpy(RegisterInfo.VatNumber.data(), VatNumber.data(), VatNumber.size());
            memcpy(RegisterInfo.PanNumber.data(), PanNumber.data(), PanNumber.size());
            memcpy(RegisterInfo.Country.data(), BusinessCountry.data(), BusinessCountry.size());
            memcpy(RegisterInfo.Address.data(), BusinessAddress.data(), BusinessAddress.size());
            memcpy(RegisterInfo.Password.data(), AccPassword.data(), AccPassword.size());
            memcpy(RegisterInfo.Ownername.data(), OwnerName.data(), OwnerName.size());

            if (PanContent.size() != 0)
                RegisterInfo.PanPdf = PanDocument.UniqueId;
            else
                RegisterInfo.PanPdf = UUID::InvalidUUID();

            if (BusinessFrontPhotoContent.size() != 0)
                RegisterInfo.BusinessFrontImage = BusinessFrontPhoto.UniqueId;
            else
                RegisterInfo.BusinessFrontImage = UUID::InvalidUUID();

            if (BusinessCertificationContent.size() != 0)
                RegisterInfo.VatCertificatePdf = BusinessCertificateDocument.UniqueId;
            else
                RegisterInfo.VatCertificatePdf = UUID::InvalidUUID();

            std::cout << "BusinessCertificationContent.size()= " << BusinessCertificationContent.size() << "\n";

            Insert(Backend.Database, RegisterInfo);

            RegisterAccountContactFormQuery Query;
            auto Forms = GetRegisterAccountContactForms(Backend.Database, Query);
            std::cout << "Forms Count: " << Forms.size() << "\n";

            break;
        }
        case ContactSubjectType::Issue:
        {
            break;
        }
        case ContactSubjectType::Help:
        {
            break;
        }
        case ContactSubjectType::Billing:
        {
            break;
        }
        case ContactSubjectType::Other:
        {
            break;
        }
        default:
            break;
        }

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AdminContactFormsQuery(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
        {
            std::cout << "Invalid Json for AdminContactFormsQuery\n";
            return crow::response(400);
        }
        std::cout << "Valid Json for AdminContactFormsQuery\n";

        crow::json::wvalue Result;
        crow::response res;

        ContactFormQuery Query;

        auto Forms = GetContactForms(Backend.Database, Query);

        for (int i = 0; i < Forms.size(); i++)
        {
            Result["Forms"][i]["UUID"] = Forms[i].UniqueId.ToString();
            Result["Forms"][i]["PhoneNumber"] = std::string(Forms[i].PhoneNumber.data());
            Result["Forms"][i]["Email"] = std::string(Forms[i].Email.data());
            Result["Forms"][i]["Type"] = ContactSubjectTypeToStr(Forms[i].Type);
            Result["Forms"][i]["CreatedAt"] = FormatDateForHtml(NepBill::ToUnixTime(Forms[i].CreatedAt));
            Result["Forms"][i]["State"] = ContactStatesToStr(Forms[i].State);
            std::cout << Forms[i].Message.data() << "\n";
            std::cout << "Email: " << Forms[i].Email.data() << "\n";
        }
        Result["Count"] = Forms.size();

        std::cout << Forms.size() << "\n";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AdminContactFormQueryDetail(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
        {
            std::cout << "Invalid Json for AdminContactFormsQuery\n";
            return crow::response(400);
        }
        std::cout << "Valid Json for AdminContactFormsQuery\n";

        crow::json::wvalue Result;
        crow::response res;

        ContactFormQuery Query;
        std::string AccountUUIDString = Json["UUID"].s();
        Query.UniqueId = UUID::FromString(std::string_view(AccountUUIDString.c_str()));

        auto Form = GetContactForms(Backend.Database, Query)[0];
        Result["Email"] = std::string(Form.Email.data());
        Result["PhoneNumber"] = std::string(Form.PhoneNumber.data());
        Result["Message"] = std::string(Form.Message.data());
        Result["State"] = std::string(ContactStatesToStr(Form.State));
        Result["Type"] = std::string(ContactSubjectTypeToStr(Form.Type));
        Result["CreatedAt"] = std::string(FormatDateForHtml(Form.CreatedAt));

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AdminContactFormEditSave(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
        {
            std::cout << "Invalid Json for AdminContactFormsQuery\n";
            return crow::response(400);
        }
        std::cout << "Valid Json for AdminContactFormEditSave\n";

        crow::json::wvalue Result;
        crow::response res;

        ContactFormQuery Query;
        std::string AccountUUIDString = Json["UUID"].s();
        Query.UniqueId = UUID::FromString(std::string_view(AccountUUIDString.c_str()));

        UpdateContactFormQuery UpdateQuery;
        UpdateQuery.UniqueId = UUID::FromString(std::string_view(AccountUUIDString.c_str()));
        if (Json.has("Email"))
            UpdateQuery.Email = Json["Email"].s();
        if (Json.has("PhoneNumber"))
            UpdateQuery.PhoneNumber = Json["PhoneNumber"].s();
        if (Json.has("State"))
            UpdateQuery.State = StrToContactStates(Json["State"].s());

        UpdateContactForm(Backend.Database, UpdateQuery);

        // if(Json.has(Email))
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AdminContactFormRegisterQuery(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
        {
            std::cout << "Invalid Json for AdminContactFormsQuery\n";
            return crow::response(400);
        }
        std::cout << "Valid Json for AdminContactFormRegisterQuery\n";

        crow::json::wvalue Result;
        crow::response res;

        RegisterAccountContactFormQuery Query;
        std::string ContactUUIDStr = Json["UUID"].s();
        Query.ContactID = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        std::cout << ContactUUIDStr << " Try RegisterAccountInfo\n";

        auto RegisterAccountInfoGet = GetRegisterAccountContactForms(Backend.Database, Query);

        if (RegisterAccountInfoGet.size() == 0)
        {
            std::cout << "Failed RegisterAccountInfo\n";
            Result["Success"] = false;
            Result["Error"] = "Failed RegisterAccountInfo";
        }
        else
        {
            auto RegisterAccountInfo = RegisterAccountInfoGet[0];
            Result["BusinessName"] = std::string(RegisterAccountInfo.Name.data());
            Result["Ownername"] = std::string(RegisterAccountInfo.Ownername.data());
            Result["PanNumber"] = std::string(RegisterAccountInfo.PanNumber.data());
            Result["VatNumber"] = std::string(RegisterAccountInfo.VatNumber.data());
            Result["Country"] = std::string(RegisterAccountInfo.Country.data());
            Result["Address"] = std::string(RegisterAccountInfo.Address.data());

            std::array<char, kPasswordHashLength> PasswordHash;
            EncryptionManager EncryptionManager;
            EncryptionManager.createPasswordHash(std::string(RegisterAccountInfo.Password.data()),
                                                 PasswordHash);
            Result["PasswordHash"] = std::string(PasswordHash.data());
            std::cout << "Success RegisterAccountInfo\n";

            // A valid 1x1 black JPEG image byte array
            std::vector<uint8_t> BlackImage = {
                0xFF, 0xD8, 0xFF, 0xDB, 0x00, 0x43, 0x00, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03,
                0x03, 0x03, 0x03, 0x04, 0x03, 0x03, 0x04, 0x05, 0x08, 0x05, 0x05, 0x04, 0x04, 0x05,
                0x0A, 0x07, 0x07, 0x06, 0x08, 0x0C, 0x0A, 0x0C, 0x0C, 0x0B, 0x0A, 0x0B, 0x0B, 0x0D,
                0x0E, 0x12, 0x10, 0xD4, 0x0E, 0x0F, 0x11, 0x0E, 0x0B, 0x0B, 0x10, 0x16, 0x10, 0x11,
                0x13, 0x14, 0x15, 0x15, 0x15, 0x0C, 0x0F, 0x17, 0x18, 0x16, 0x14, 0x18, 0x12, 0x14,
                0x15, 0x14, 0xFF, 0xC0, 0x00, 0x0B, 0x08, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x11,
                0x00, 0xFF, 0xC4, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xFF, 0xC4, 0x00, 0x14, 0x10, 0x01,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0xFF, 0xDA, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3F, 0x00, 0x7F, 0x00, 0x00,
                0x01, 0xFF, 0xD9};

            // 2. Fetch and encode PAN PDF safely (Your original structure)
            if (RegisterAccountInfo.PanPdf != UUID::InvalidUUID())
            {
                PdfDocumentQuery PanQuery;
                PanQuery.UniqueId = RegisterAccountInfo.PanPdf;
                auto PanPdfs = GetPdfDocuments(Backend.Database, PanQuery);

                if (!PanPdfs.empty())
                {
                    Result["PanPdf"] = crow::utility::base64encode(PanPdfs[0].Data.data(), PanPdfs[0].Data.size());
                }
                else
                {
                    Result["PanPdf"] = "";
                }
            }
            // 3. FIX: Fetch and encode VAT PDF safely (Prevents index out-of-bounds crash)
            if (RegisterAccountInfo.VatCertificatePdf != UUID::InvalidUUID())
            {
                PdfDocumentQuery VatCertQuery;
                VatCertQuery.UniqueId = RegisterAccountInfo.VatCertificatePdf;
                auto VatPdfs = GetPdfDocuments(Backend.Database, VatCertQuery);

                if (!VatPdfs.empty())
                {
                    Result["VatPdf"] = crow::utility::base64encode(VatPdfs[0].Data.data(), VatPdfs[0].Data.size());
                }
                else
                {
                    Result["VatPdf"] = crow::utility::base64encode(BlackImage.data(), BlackImage.size());
                }
            }

            if (RegisterAccountInfo.VatCertificatePdf != UUID::InvalidUUID())
            {
                // 4. FIX: Fetch and encode Business Front Image safely (Prevents index out-of-bounds crash)
                ImageQuery BusinessFrontImageQuery;
                // Note: Make sure to assign the query criteria if needed, e.g.:
                BusinessFrontImageQuery.UniqueId = RegisterAccountInfo.BusinessFrontImage;
                auto BusinessImages = GetImages(Backend.Database, BusinessFrontImageQuery);

                if (!BusinessImages.empty())
                {
                    Result["BusinessFrontImage"] = crow::utility::base64encode(BusinessImages[0].Data.data(), BusinessImages[0].Data.size());
                }
                else
                {
                    Result["BusinessFrontImage"] = crow::utility::base64encode(BlackImage.data(), BlackImage.size());
                }
            }

            std::cout << "?Vat Invalid: " << bool(RegisterAccountInfo.VatCertificatePdf == UUID::InvalidUUID()) << "\n";

            AccountQuery AccountQuery;
            AccountQuery.ContactFormID = Query.ContactID;

            auto AccountAssociated = GetAccounts(Backend.Database, AccountQuery);

            if (AccountAssociated.size() == 0)
                Result["AccountAssociated"] = false;
            else
                Result["AccountAssociated"] = true;
            Result["Success"] = true;
            Result["Error"] = "None";
        }
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AdminContactFormRegisterApprove(NepBill::App &Backend, const crow::request &Req)
    {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
        {
            std::cout << "Invalid Json for AdminContactFormsQuery\n";
            return crow::response(400);
        }
        std::cout << "Valid Json for AdminContactFormEditSave\n";

        crow::json::wvalue Result;
        crow::response res;

        RegisterAccountContactFormQuery Query;
        std::string ContactUUIDStr = Json["UUID"].s();
        Query.ContactID = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));
        auto RegisterAccountInfo = GetRegisterAccountContactForms(Backend.Database, Query)[0];

        Business NewBusinessInfo;
        NewBusinessInfo.RegisterAccountFormId = RegisterAccountInfo.ContactID;

        Account NewAccountInfo;
        NewAccountInfo.BusinessID = NewBusinessInfo.UniqueId;
        NewAccountInfo.ContactFormID = RegisterAccountInfo.ContactID;
        NewAccountInfo.Role = AccountRoles::BusinessOwner;

        EncryptionManager EncryptionManager;
        EncryptionManager.createPasswordHash(std::string(RegisterAccountInfo.Password.data()),
                                             NewAccountInfo.PasswordHash);

        Insert(Backend.Database, NewBusinessInfo);
        Insert(Backend.Database, NewAccountInfo);

        Result["Success"] = true;
        Result["Role"] = AccountRolesToStr(NewAccountInfo.Role);

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }
}
