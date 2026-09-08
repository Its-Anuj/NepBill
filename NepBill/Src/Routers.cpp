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
            Result["Message"] = "No Token Found!";
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
            Result["LoggedIn"] = false;
            Result["Message"] = "Try Failed";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 200;

            return res;
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

        auto &Account = Accounts[0];

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

        // 1. Format clearance header matching the original cookie path and flags
        // (Omit 'Secure' on local HTTP and use SameSite=Lax)
        std::string cookie_header = "auth_token=; Path=/; HttpOnly; SameSite=Lax; "
                                    "Max-Age=0; Expires=Thu, 01 Jan 1970 00:00:00 GMT";

        res.add_header("Set-Cookie", cookie_header);

        // 2. Add CORS headers for Linux/Brave cross-origin cookie deletion
        std::string origin = Req.get_header_value("Origin");
        if (!origin.empty())
        {
            res.add_header("Access-Control-Allow-Origin", origin);
            res.add_header("Access-Control-Allow-Credentials", "true");
        }

        crow::json::wvalue json;
        json["Success"] = true;
        json["Message"] = "Logged out successfully";

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

                    std::string cookie_header = "auth_token=" + token +
                                                "; Path=/; HttpOnly; SameSite=Lax; Max-Age=" +
                                                std::to_string(MaxLoginCookieAge);

                    res.add_header("Set-Cookie", cookie_header);

                    // Handle CORS credentials explicitly for Linux Brave
                    std::string origin = Req.get_header_value("Origin");
                    if (!origin.empty())
                    {
                        res.add_header("Access-Control-Allow-Origin", origin);
                        res.add_header("Access-Control-Allow-Credentials", "true");
                    }

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

    crow::response QuerySuppliersIncomingOrders(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["UUID"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        PurchaseOrderQuery RecentPurchaseOrderQuery;
        RecentPurchaseOrderQuery.BusinessID = UserAccount->BusinessID;
        RecentPurchaseOrderQuery.CreatedBefore = ToUnixTime(TodayTime);
        RecentPurchaseOrderQuery.CreatedAfter = ToUnixTime(OneWeekAgoTime);
        auto RecentPurchaseOrders = GetPurchaseOrders(Backend.Database, RecentPurchaseOrderQuery);

        Result["Count"] = RecentPurchaseOrders.size();
        for (size_t i = 0; i < RecentPurchaseOrders.size(); i++)
        {
            PurchaseOrderLineQuery LineQueries;
            LineQueries.PurchaseOrderID = RecentPurchaseOrders[i].UniqueID;

            auto Queries = GetPurchaseOrderLines(Backend.Database, LineQueries);

            Result["Orders"][i]["Count"] = Queries.size();

            for (int j = 0; j < Queries.size(); j++)
            {
                ItemQuery ItemQuery;
                ItemQuery.UniqueID = Queries[j].ItemID;
                auto Items = GetItems(Backend.Database, ItemQuery);

                if (Items.size() == 0)
                {
                    Result["State"] = false;
                    Result["Message"] = "Item For Purchase Order Line: " + Queries[i].UniqueID.ToString() + " ,doesnot exist";

                    // FIX: Set the content type to JSON and dump the JSON object to a string string
                    res.add_header("Content-Type", "application/json");
                    res.write(Result.dump());
                    res.code = 400;

                    return res;
                }
                auto Item = Items[0];

                Result["Orders"][i]["Lines"][j]["OrderedQuantity"] = Queries[i].OrderedQuantity;
                Result["Orders"][i]["Lines"][j]["UnitPrice"] = Queries[i].UnitPrice;
                Result["Orders"][i]["Lines"][j]["Name"] = std::string(Item.Name.data());
                Result["Orders"][i]["Lines"][j]["ReceivedQuantity"] = Queries[i].ReceivedQuantity;
            }

            auto TodayIntTime = ToUnixTime(TodayTime);
            auto CreatedAtIntTime = ToUnixTime(RecentPurchaseOrders[i].CreatedAt);

            auto Diff = TodayIntTime - CreatedAtIntTime;

            SuppliersQuery OrderSuppliersQuery;
            OrderSuppliersQuery.UnqiueId = RecentPurchaseOrders[i].SupplierID;
            auto OrderSuppliers = GetSuppliers(Backend.Database, OrderSuppliersQuery);

            if (OrderSuppliers.size() == 0)
            {
                Result["State"] = false;
                Result["Message"] = "Suppliers For Purchase Order: " + RecentPurchaseOrders[i].UniqueID.ToString() + " ,doesnot exist";

                // FIX: Set the content type to JSON and dump the JSON object to a string string
                res.add_header("Content-Type", "application/json");
                res.write(Result.dump());
                res.code = 400;

                return res;
            }
            auto OrderSupplier = OrderSuppliers[0];

            Result["Orders"][i]["TimeAgo"] = Diff;
            Result["Orders"][i]["State"] = PurchaseOrderStateToStr(RecentPurchaseOrders[i].State);
            Result["Orders"][i]["Name"] = std::string(OrderSupplier.Name.data());
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryItemCategoryByName(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string ItemCategoryName = Json["CategoryName"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemCategoryQuery ItemCategoryQuery;
        ItemCategoryQuery.Name = ItemCategoryName;
        ItemCategoryQuery.SortField = ItemCategorySortField::Name;

        auto ItemCategories = GetItemCategories(Backend.Database, ItemCategoryQuery);

        Result["Count"] = ItemCategories.size();
        for (int i = 0; i < ItemCategories.size(); i++)
        {
            Result["Categories"][i]["Name"] = ItemCategories[i].Name;
            Result["Categories"][i]["UUID"] = ItemCategories[i].UniqueID.ToString();
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryItemByName(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string ItemName = Json["Name"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemQuery ItemQuery;
        ItemQuery.Name = ItemName;
        ItemQuery.SortField = ItemSortField::Name;

        std::vector<Item> Items = GetItems(Backend.Database, ItemQuery);

        Result["Count"] = Items.size();
        for (int i = 0; i < Items.size(); i++)
        {
            Result["Categories"][i]["Name"] = Items[i].Name.data();
            Result["Categories"][i]["UUID"] = Items[i].UniqueID.ToString();
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AddItem(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string ItemName = Json["ItemName"].s();
        bool TrackStock = Json["TrackStockBool"].b();
        uint32_t LowStockThreshold = Json["StockThreshold"].i();
        std::string CategoryName = Json["ItemCategoryName"].s();
        std::string Description = Json["Desc"].s();
        MeasurementUnit Unit = (MeasurementUnit)Json["MeasurementUnit"].i();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemCategoryQuery CheckCategoryQuery;
        CheckCategoryQuery.Name = CategoryName;
        CheckCategoryQuery.SortField = ItemCategorySortField::Name;

        auto QueryCategories = GetItemCategories(Backend.Database, CheckCategoryQuery);
        ItemCategory *ActiveItemCategory = nullptr;

        if (QueryCategories.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "Item Category not Found!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }
        ActiveItemCategory = &QueryCategories[0];

        ItemQuery CheckItemQuery;
        CheckItemQuery.Name = ItemName;
        CheckItemQuery.SortField = NepBill::ItemSortField::Name;

        auto QueryItems = GetItems(Backend.Database, CheckItemQuery);

        if (QueryItems.size() > 0)
        {
            Result["State"] = false;
            Result["Message"] = "Item Already Exists!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        Item NewItem;
        NewItem.BusinessID = UserAccount->BusinessID;
        NewItem.CategoryId = ActiveItemCategory->UniqueID;
        NewItem.Name = ItemName;
        NewItem.Description = Description;
        NewItem.LowStockThresold = LowStockThreshold;
        NewItem.TracksStock = TrackStock;
        NewItem.StockUnit = Unit;

        Insert(Backend.Database, NewItem);

        std::cout << "Added Item: " << ItemName << "\n";

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AddItemCategory(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string CategoryName = Json["NewCategoryName"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemCategoryQuery CheckCategoryQuery;
        CheckCategoryQuery.Name = CategoryName;
        CheckCategoryQuery.BusinessID = UserAccount->BusinessID;
        CheckCategoryQuery.SortField = ItemCategorySortField::Name;

        auto QueryCategories = GetItemCategories(Backend.Database, CheckCategoryQuery);

        if (QueryCategories.size() > 0)
        {
            Result["State"] = false;
            Result["Message"] = "Category For " + CategoryName + " Already Exists!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 200;

            return res;
        }

        ItemCategory NewItemCategory;
        NewItemCategory.BusinessID = UserAccount->BusinessID;
        NewItemCategory.Name = CategoryName;
        Insert(Backend.Database, NewItemCategory);

        Result["State"] = true;
        Result["Message"] = "Succesfuly Added Category: " + CategoryName;

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryItemStock(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemQuery ItemStockQueries;
        ItemStockQueries.BusinessID = UserAccount->BusinessID;
        ItemStockQueries.SortField = ItemSortField::Name;

        auto ItemStock = GetItems(Backend.Database, ItemStockQueries);

        Result["Count"] = ItemStock.size();
        for (int i = 0; i < ItemStock.size(); i++)
        {
            ItemCategoryQuery ItemCategoryQuery;
            ItemCategoryQuery.UniqueID = ItemStock[i].CategoryId;

            auto QueryCategories = GetItemCategories(Backend.Database, ItemCategoryQuery);
            std::optional<ItemCategory *> Category = std::nullopt;

            if (QueryCategories.size() == 0)
            {
                Result["State"] = false;
                Result["Message"] = "Querying Item Category For: " + ItemStock[i].Name + "  Info Failed!";

                // FIX: Set the content type to JSON and dump the JSON object to a string string
                res.add_header("Content-Type", "application/json");
                res.write(Result.dump());
                res.code = 400;

                return res;
            }
            Category = &QueryCategories[0];

            ItemStockLedgerQuery CurrentItemStockLedgerQuery;
            CurrentItemStockLedgerQuery.ItemId = ItemStock[i].UniqueID;

            auto ItemStockLedgers = GetItemStockLedgers(Backend.Database, CurrentItemStockLedgerQuery);

            uint32_t CurrentStock = 0;
            for (auto &StockLedger : ItemStockLedgers)
                CurrentStock += StockLedger.StockDelta;

            Result["Item"][i]["Name"] = ItemStock[i].Name;
            Result["Item"][i]["ItemUUID"] = ItemStock[i].UniqueID.ToString();
            Result["Item"][i]["LowStockThresold"] = ItemStock[i].LowStockThresold;
            Result["Item"][i]["TracksStock"] = (ItemStock[i].TracksStock) ? "true" : "false";
            Result["Item"][i]["CategoryName"] = Category.value()->Name;
            Result["Item"][i]["Description"] = ItemStock[i].Description;
            Result["Item"][i]["CurrentStock"] = CurrentStock;
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryItemDetail(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        std::string ItemUUIDStr = Json["ItemId"].s();
        auto ItemId = UUID::FromString(std::string_view(ItemUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemQuery ItemQuery;
        ItemQuery.BusinessID = UserAccount->BusinessID;
        ItemQuery.UniqueID = ItemId;

        auto ItemQuerieis = GetItems(Backend.Database, ItemQuery);

        if (ItemQuerieis.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "Item Query Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        auto Item = &ItemQuerieis[0];

        ItemCategoryQuery ItemCategoryQuery;
        ItemCategoryQuery.BusinessID = UserAccount->BusinessID;
        ItemCategoryQuery.UniqueID = Item->CategoryId;

        auto ItemCategories = GetItemCategories(Backend.Database, ItemCategoryQuery)[0];

        Result["Name"] = Item->Name;
        Result["CategoryName"] = ItemCategories.Name;
        Result["LowStockThresold"] = Item->LowStockThresold;
        Result["TracksStock"] = Item->TracksStock;
        Result["StockUnit"] = MeasurementUnitToStr(Item->StockUnit);
        Result["DefaultPurchaseUnit"] = PurchaseUnitToStr(Item->DefaultPurchaseUnit);
        Result["DefaultPurchaseConversion"] = Item->DefaultPurchaseConversion;
        Result["Description"] = Item->Description;

        // In Future add a section where we show like what what purchase oders, stock changes happened here, and
        // orders related to this item make a placeholder for that

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryItemStockUnit(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string ItemName = Json["ItemName"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        ItemQuery CheckItemQuery;
        CheckItemQuery.BusinessID = UserAccount->BusinessID;
        CheckItemQuery.Name = ItemName;
        CheckItemQuery.SortField = ItemSortField::Name;
        CheckItemQuery.ExactMatch = true;

        auto QueryItems = GetItems(Backend.Database, CheckItemQuery);

        if (QueryItems.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "Given Item doesnot exist!!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }
        auto Item = &QueryItems[0];

        Result["State"] = true;
        Result["Message"] = "Succesful!";
        Result["Stock Unit"] = (uint32_t)Item->StockUnit;

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AddSuppliers(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string Name = Json["Name"].s();
        std::string PanNumber = Json["PanNumber"].s();
        std::string PhoneNumber = Json["PhoneNumber"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        SuppliersQuery CheckSuppliersQuery;
        CheckSuppliersQuery.BusinessID = UserAccount->BusinessID;
        CheckSuppliersQuery.Name = Name;

        auto QuerySuppliers = GetSuppliers(Backend.Database, CheckSuppliersQuery);
        if (QuerySuppliers.size() > 0)
        {
            Result["State"] = false;
            Result["Message"] = "Name with this supplier already exists!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        Suppliers NewSupplier;
        NewSupplier.BusinessID = UserAccount->BusinessID;
        NewSupplier.Name = Name;
        NewSupplier.PanNumber = PanNumber;
        NewSupplier.PhoneNumber = PhoneNumber;
        Insert(Backend.Database, NewSupplier);
        std::cout << "Added Supplier: " << Name << "\n";

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QuerySuppliers(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        SuppliersQuery CheckSuppliersQuery;
        CheckSuppliersQuery.BusinessID = UserAccount->BusinessID;

        auto QuerySuppliers = GetSuppliers(Backend.Database, CheckSuppliersQuery);
        Result["Count"] = QuerySuppliers.size();

        for (int i = 0; i < QuerySuppliers.size(); i++)
        {
            Result["Supplier"][i]["Name"] = QuerySuppliers[i].Name;
            Result["Supplier"][i]["PanNumber"] = QuerySuppliers[i].PanNumber;
            Result["Supplier"][i]["PhoneNumber"] = QuerySuppliers[i].PhoneNumber;
            Result["Supplier"][i]["OpeningBalance"] = QuerySuppliers[i].OpeningBalance;
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QuerySuppliersByNameFilter(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        std::string Name = Json["Name"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        SuppliersQuery SuppliersQuery;
        SuppliersQuery.BusinessID = UserAccount->BusinessID;
        SuppliersQuery.Name = Name;
        SuppliersQuery.SortField = SuppliersSortField::Name;

        auto Suppliers = GetSuppliers(Backend.Database, SuppliersQuery);

        Result["Count"] = Suppliers.size();
        for (int i = 0; i < Suppliers.size(); i++)
        {
            Result["Categories"][i]["Name"] = Suppliers[i].Name;
            Result["Categories"][i]["UUID"] = Suppliers[i].UnqiueId.ToString();
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AddPurchaseOrder(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        std::string SupplierUUIDStr = Json["SupplierUUID"].s();
        auto SupplierId = UUID::FromString(std::string_view(SupplierUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        PurchaseOrder PurchaseOrder;
        PurchaseOrder.BusinessID = UserAccount->BusinessID;
        PurchaseOrder.SupplierID = SupplierId;

        std::vector<PurchaseOrderLine> OrderLines;

        for (auto &Line : Json["Lines"])
        {
            PurchaseOrderLine NewOrderLine;
            std::string ItemUUIDStr = Line["ItemUUID"].s();

            ItemQuery ValidateItemQuery;
            ValidateItemQuery.BusinessID = UserAccount->BusinessID;
            ValidateItemQuery.UniqueID = UUID::FromString(std::string_view(ItemUUIDStr.c_str()));

            auto ItemQueries = GetItems(Backend.Database, ValidateItemQuery);

            if (ItemQueries.size() == 0)
            {
                Result["State"] = false;
                Result["Message"] = "Line Count: " +
                                    std::to_string(OrderLines.size()) + " Has a non existent item";

                // FIX: Set the content type to JSON and dump the JSON object to a string string
                res.add_header("Content-Type", "application/json");
                res.write(Result.dump());
                res.code = 400;

                return res;
            }

            NewOrderLine.ItemID = ValidateItemQuery.UniqueID.value();
            NewOrderLine.OrderedQuantity = Line["OrderedQuantity"].i();
            NewOrderLine.PurchaseOrderID = PurchaseOrder.UniqueID;
            NewOrderLine.PurchaseToStockConversion = Line["PurchaseToStockConversion"].i();
            NewOrderLine.PurchaseUnit = (PurchaseUnit)Line["PurchaseUnit"].i();
            NewOrderLine.StockUnit = ItemQueries[0].StockUnit;
            NewOrderLine.Remarks = Line["Remarks"].s();
            NewOrderLine.UnitPrice = Line["UnitPrice"].i();
            NewOrderLine.DiscountPercent = Line["DiscountPercent"].i();

            OrderLines.push_back(NewOrderLine);
        }

        Insert(Backend.Database, PurchaseOrder);

        for (auto &Line : OrderLines)
            Insert(Backend.Database, Line);

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryPurchaseOrderList(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        PurchaseOrderQuery PurchaseOrderQuery;
        PurchaseOrderQuery.BusinessID = UserAccount->BusinessID;

        auto PurchaseOrderQueries = GetPurchaseOrders(Backend.Database, PurchaseOrderQuery);

        Result["Count"] = PurchaseOrderQueries.size();

        for (int i = 0; i < PurchaseOrderQueries.size(); i++)
        {
            auto Order = &PurchaseOrderQueries[i];

            SuppliersQuery OrderSuppliersQuery;
            OrderSuppliersQuery.BusinessID = UserAccount->BusinessID;
            OrderSuppliersQuery.UnqiueId = Order->SupplierID;

            auto OrderSupplierQueries = GetSuppliers(Backend.Database, OrderSuppliersQuery);
            auto &OrderSupplier = OrderSupplierQueries[0];

            PurchaseOrderLineQuery OrderLineQueries;
            OrderLineQueries.PurchaseOrderID = Order->UniqueID;

            auto OrderLineItemQueries = GetPurchaseOrderLines(Backend.Database, OrderLineQueries);

            Result["Orders"][i]["SupplierName"] = OrderSupplier.Name;
            Result["Orders"][i]["SupplierUUID"] = OrderSupplier.UnqiueId.ToString();
            Result["Orders"][i]["OrderUUID"] = Order->UniqueID.ToString();
            Result["Orders"][i]["LinesCount"] = OrderLineItemQueries.size();
            Result["Orders"][i]["State"] = PurchaseOrderStateToStr(Order->State);
            Result["Orders"][i]["CreatedAt"] = FormatDateForHtml(Order->CreatedAt);
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryPurchaseOrderDetail(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        std::string OrderUUIDStr = Json["OrderId"].s();
        auto OrderId = UUID::FromString(std::string_view(OrderUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        PurchaseOrderQuery PurchaseOrderQuery;
        PurchaseOrderQuery.BusinessID = UserAccount->BusinessID;
        PurchaseOrderQuery.UniqueID = OrderId;

        auto PurchaseOrderQueries = GetPurchaseOrders(Backend.Database, PurchaseOrderQuery);

        if (PurchaseOrderQueries.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "Given OrderId Has no PurchaseOrder!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        auto Order = &PurchaseOrderQueries[0];

        SuppliersQuery OrderSuppliersQuery;
        OrderSuppliersQuery.BusinessID = UserAccount->BusinessID;
        OrderSuppliersQuery.UnqiueId = Order->SupplierID;

        auto OrderSupplierQueries = GetSuppliers(Backend.Database, OrderSuppliersQuery);
        auto &OrderSupplier = OrderSupplierQueries[0];

        PurchaseOrderLineQuery OrderLineQueries;
        OrderLineQueries.PurchaseOrderID = Order->UniqueID;

        auto OrderLineItemQueries = GetPurchaseOrderLines(Backend.Database, OrderLineQueries);

        Result["LinesCount"] = OrderLineItemQueries.size();
        Result["SupplierName"] = OrderSupplier.Name;
        Result["SupplierUUID"] = OrderSupplier.UnqiueId.ToString();
        Result["PurchaseOrderState"] = PurchaseOrderStateToStr(Order->State);
        Result["CreatedAt"] = FormatDateForHtml(Order->CreatedAt);

        for (int i = 0; i < OrderLineItemQueries.size(); i++)
        {
            auto Line = &OrderLineItemQueries[i];

            ItemQuery LineItemQuery;
            LineItemQuery.UniqueID = Line->ItemID;

            auto ItemQueries = GetItems(Backend.Database, LineItemQuery);

            Result["Lines"][i]["LineUUID"] = Line->UniqueID.ToString();
            Result["Lines"][i]["ItemUUID"] = ItemQueries[0].UniqueID.ToString();
            Result["Lines"][i]["ItemName"] = ItemQueries[0].Name;
            Result["Lines"][i]["OrderedQuantity"] = Line->OrderedQuantity;
            Result["Lines"][i]["ReceivedQuantity"] = Line->ReceivedQuantity;
            Result["Lines"][i]["PurchaseToStockConversion"] = Line->PurchaseToStockConversion;
            Result["Lines"][i]["UnitPrice"] = Line->UnitPrice;
            Result["Lines"][i]["DiscountPercent"] = Line->DiscountPercent;
            Result["Lines"][i]["Remarks"] = Line->Remarks;
            Result["Lines"][i]["PurchaseUnit"] = PurchaseUnitToStr(Line->PurchaseUnit);
            Result["Lines"][i]["StockUnit"] = MeasurementUnitToStr(Line->StockUnit);
        }

        if (Order->State != PurchaseOrderState::Draft)
        {
            ItemInvoiceQuery ItemInvoiceQuery;
            ItemInvoiceQuery.UniqueID = Order->ItemInvoiceId;

            auto ItemInvoiceQueries = GetItemInvoices(Backend.Database, ItemInvoiceQuery);

            if (ItemInvoiceQueries.size() > 0)
                auto &Invoice = ItemInvoiceQueries[0];

            Result["ItemInvoiceId"] = ItemInvoiceQueries[0].UniqueID.ToString();
            Result["InvoiceState"] = InvoiceStatesToStr(ItemInvoiceQueries[0].State);
            Result["VatPercent"] = ItemInvoiceQueries[0].VatPercent;
        }

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryPurchaseOrderLineDetail(NepBill::App &Backend, const crow::request &Req)
    {
        return crow::response();
    }

    crow::response ChangeOrderState(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        std::string OrderUUIDStr = Json["OrderId"].s();
        auto OrderId = UUID::FromString(std::string_view(OrderUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        PurchaseOrderQuery PurchaseOrderQuery;
        PurchaseOrderQuery.BusinessID = UserAccount->BusinessID;
        PurchaseOrderQuery.UniqueID = OrderId;

        auto PurchaseOrderQueries = GetPurchaseOrders(Backend.Database, PurchaseOrderQuery);

        if (PurchaseOrderQueries.size() == 0)
        {
            Result["State"] = false;
            Result["Message"] = "Given OrderId Has no PurchaseOrder!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        auto Order = &PurchaseOrderQueries[0];

        SuppliersQuery OrderSuppliersQuery;
        OrderSuppliersQuery.BusinessID = UserAccount->BusinessID;
        OrderSuppliersQuery.UnqiueId = Order->SupplierID;

        auto OrderSupplierQueries = GetSuppliers(Backend.Database, OrderSuppliersQuery);
        auto &OrderSupplier = OrderSupplierQueries[0];

        PurchaseOrderLineQuery OrderLineQueries;
        OrderLineQueries.PurchaseOrderID = Order->UniqueID;

        auto OrderLineItemQueries = GetPurchaseOrderLines(Backend.Database, OrderLineQueries);

        for (int i = 0; i < OrderLineItemQueries.size(); i++)
        {
            ItemStockLedger UpdatedLedger;
            UpdatedLedger.AccountID = UserAccount->UniqueId;
            UpdatedLedger.BusinessID = UserAccount->BusinessID;
            UpdatedLedger.ItemId = OrderLineItemQueries[i].ItemID;
            UpdatedLedger.PurchaseOrderID = Order->UniqueID;
            UpdatedLedger.Reason = StockMovementReason::PurchaseDelivery;
            UpdatedLedger.StockDelta = OrderLineItemQueries[i].ReceivedQuantity;
            // Set CreatedAt to current system time
            Insert(Backend.Database, UpdatedLedger);

            PurchaseOrderLineUpdate Update;
            Update.ReceivedQuantity = Json["LineReceivedQuantity"][i].i();
            UpdatePurchaseOrderLine(Backend.Database, OrderLineItemQueries[i].UniqueID, Update);
        }

        ItemInvoice Invoice;
        Invoice.SenderId = UserAccount->BusinessID;
        Invoice.RecieverId = OrderSupplier.UnqiueId;
        Invoice.State = StrToInvoiceStates(Json["InvoiceInfo"]["paymentState"].s());
        Invoice.VatPercent = Json["InvoiceInfo"]["vatPercent"].i();
        Invoice.Total = Json["InvoiceInfo"]["grandTotal"].i();
        Invoice.SenderType.Type = FinancialPaymentReceipientType::OurBusiness;
        Invoice.SenderType.ServiceType = ServiceType::Hotel;
        Invoice.RecieverType.Type = FinancialPaymentReceipientType::Supplier;
        Invoice.RecieverType.ServiceType = ServiceType::Hotel;

        Insert(Backend.Database, Invoice);

        PurchaseOrderUpdate OrderUpdate;
        OrderUpdate.State = (PurchaseOrderState)Json["ChangeStateId"].i();
        OrderUpdate.ItemInvoiceId = Invoice.UniqueID;

        UpdatePurchaseOrder(Backend.Database, Order->UniqueID, OrderUpdate);

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response AddRoomInfo(NepBill::App &Backend, const crow::request &Req)
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

        std::string ContactUUIDStr = Json["AccountId"].s();
        auto UserId = UUID::FromString(std::string_view(ContactUUIDStr.c_str()));

        auto TodayTime = getCurrentTime();
        auto OneWeekAgoTime = getTimeOneWeekAgo();

        AccountQuery UserAccountQuery;
        UserAccountQuery.UniqueId = UserId;
        std::optional<Account> UserAccount = std::nullopt;

        auto Accounts = GetAccounts(Backend.Database, UserAccountQuery);
        if (Accounts.size() == 1)
            UserAccount = Accounts[0];

        if (UserAccount == std::nullopt)
        {
            Result["State"] = false;
            Result["Message"] = "Querying Account Info Failed!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        std::string RoomName = Json["Name"].s();

        // Check if same room name exists or no inside same business

        RoomInfoQuery ValidateRoomInfo;
        ValidateRoomInfo.BusinessId = UserAccount->BusinessID;
        ValidateRoomInfo.Name = RoomName;

        auto ValidateRooms = GetRooms(Backend.Database, ValidateRoomInfo);
        if (ValidateRooms.size() > 0)
        {
            Result["State"] = false;
            Result["Message"] = "Room with same name exists!";

            // FIX: Set the content type to JSON and dump the JSON object to a string string
            res.add_header("Content-Type", "application/json");
            res.write(Result.dump());
            res.code = 400;

            return res;
        }

        RoomInfo NewRoom;
        NewRoom.Name = RoomName;
        NewRoom.BusinessID = UserAccount->BusinessID;
        NewRoom.Description = Json["Description"].s();
        NewRoom.BasePrice = Json["BasePrice"].i();
        NewRoom.BedCount = Json["BedCount"].i();
        NewRoom.State = RoomStates::Available;

        Insert(Backend.Database, NewRoom);

        Result["State"] = true;
        Result["Message"] = "Succesful!";

        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());
        res.code = 200;

        return res;
    }

    crow::response QueryRoomInfoDetail(NepBill::App &Backend, const crow::request &Req)
    {
        return crow::response();
    }

    crow::response QueryRoomList(NepBill::App &Backend, const crow::request &Req)
    {
        return crow::response();
    }
}
