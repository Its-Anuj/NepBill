#include "PCH.h"
#include "NepalDateConverter.h"
#include "Maths.h"
#include "NepBill.h"
#include "jwt-cpp/jwt.h"
#include "tl/expected.hpp"

const std::string JWT_COOKIE_KEY = "69f2e46b088b2b73d2a71d87cfb8813a34a819b16892e6205cfbf03738e4df45";
constexpr int MaxLoginCookieAge = 60 * 60 * 24 * 30; // 30 days

void CreateTables(pqxx::connection &Db)
{
    // 2. Start a non-transaction block (for read-only queries)
    pqxx::work tx(Db);

    tx.exec(std::string_view(NepBill::ContactFormInfo::GetCreateQuery()));
    tx.exec(std::string_view(NepBill::RegisterAccountContactFormInfo::GetCreateQuery()));
    tx.exec(std::string_view(NepBill::AccountRoleInfo::GetCreateQuery()));
    tx.commit();
    std::cout << "Created Tablees\n";
}

/**
 * @brief Parses an HTML date string (YYYY-MM-DD) from Crow JSON into a std::time_t.
 *
 * @param json_obj The crow::json::rvalue containing your data.
 * @param key The key string (e.g., "createdAfter").
 * @return std::time_t The parsed timestamp at 00:00:00 UTC/Local of that day.
 */
std::time_t ParseHtmlDate(const crow::json::rvalue &json_obj, const std::string &key)
{
    // 1. Safety check: Check if the key exists
    if (!json_obj.has(key))
    {
        throw std::invalid_argument("Key '" + key + "' missing from JSON.");
    }

    // 2. Extract raw string ("YYYY-MM-DD")
    std::string raw_date = json_obj[key].s();

    // Handle empty input from HTML (user cleared the date picker)
    if (raw_date.empty())
    {
        throw std::invalid_argument("Date field is empty.");
    }

    // 3. Initialize time structure
    std::tm t = {};
    std::istringstream ss(raw_date);

    // 4. Parse using standard HTML format
    ss >> std::get_time(&t, "%Y-%m-%d");

    if (ss.fail())
    {
        throw std::runtime_error("Failed to parse date string format: " + raw_date);
    }

    // Optional: Explicitly normalize time to midnight
    t.tm_hour = 0;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1; // Let the system determine Daylight Saving Time

    // 5. Convert to timestamp
    std::time_t timestamp = std::mktime(&t);
    if (timestamp == -1)
    {
        throw std::runtime_error("Invalid calendar date values provided.");
    }

    return timestamp;
}

/**
 * @brief Converts a standard C++ timestamp into a string format compatible with HTML <input type="date">
 *
 * @param timestamp The time_t value to convert.
 * @return std::string Formatted as "YYYY-MM-DD"
 */
std::string FormatDateForHtml(std::time_t timestamp)
{
    std::tm *t = std::localtime(&timestamp); // Use std::gmtime(&timestamp) if your DB stores UTC
    std::cout << "2\n";
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%d");
    std::cout << "3\n";
    return oss.str();
}

NepBill::ContactSubjectType GetType(const std::string &Subject)
{
    NepBill::ContactSubjectType SubjectType;

    if (Subject == "RegisterAccount")
        SubjectType = NepBill::ContactSubjectType::RegisterAccount;
    else if (Subject == "Issue")
        SubjectType = NepBill::ContactSubjectType::Issue;
    else if (Subject == "Help")
        SubjectType = NepBill::ContactSubjectType::Help;
    else if (Subject == "Billing")
        SubjectType = NepBill::ContactSubjectType::Billing;
    else if (Subject == "Other")
        SubjectType = NepBill::ContactSubjectType::Other;
    return SubjectType;
}

std::string GetStrFromType(NepBill::ContactSubjectType Subject)
{
    if (Subject == NepBill::ContactSubjectType::RegisterAccount)
        return "RegisterAccount";
    else if (Subject == NepBill::ContactSubjectType::Issue)
        return "Issue";
    else if (Subject == NepBill::ContactSubjectType::Help)
        return "Help";
    else if (Subject == NepBill::ContactSubjectType::Billing)
        return "Billing";
    else if (Subject == NepBill::ContactSubjectType::Other)
        return "Other";
}

std::string GetStrFromContactState(NepBill::ContactStates Subject)
{
    if (Subject == NepBill::ContactStates::Failed)
        return "Failed";
    else if (Subject == NepBill::ContactStates::Processing)
        return "Processing";
    else if (Subject == NepBill::ContactStates::Handled)
        return "Handled";
    else if (Subject == NepBill::ContactStates::None)
        return "None";
}

bool SubmitContact(NepBill::App &Backend, const crow::json::rvalue &Json)
{
    std::string Phone =
        Json["phone"].s();

    std::string Subject =
        Json["subject"].s();

    std::string Email =
        Json["email"].s();

    std::string Message =
        Json["message"].s();

    NepBill::ContactSubjectType SubjectType = GetType(Subject);

    NepBill::ContactFormInfo FormInfo;
    memcpy(FormInfo.PhoneNumber.data(), Phone.data(), Phone.size());
    memcpy(FormInfo.Message.data(), Message.data(), Message.size());
    memcpy(FormInfo.Email.data(), Email.data(), Email.size());
    FormInfo.Type = SubjectType;
    NepBill::Insert(Backend.Database, FormInfo, true);

    NepBill::Insert(Backend.Database, FormInfo, true);

    std::cout << Email << "\n";
    std::cout << Phone << "\n";
    std::cout << "Submitedd" << "\n";

    switch (SubjectType)
    {
    case NepBill::ContactSubjectType::RegisterAccount:
    {
        std::string BusinessName =
            Json["businessName"].s();

        std::string Country =
            Json["country"].s();

        std::string Address =
            Json["address"].s();

        std::string VatNumber =
            Json["vatNumber"].s();

        std::string PanNumber =
            Json["panNumber"].s();

        NepBill::RegisterAccountContactFormInfo RegisterFormInfo;
        memcpy(RegisterFormInfo.Address.data(), Address.data(), Address.size());
        memcpy(RegisterFormInfo.Country.data(), Country.data(), Country.size());
        memcpy(RegisterFormInfo.VatNumber.data(), VatNumber.data(), VatNumber.size());
        memcpy(RegisterFormInfo.Name.data(), BusinessName.data(), BusinessName.size());
        memcpy(RegisterFormInfo.PrimaryPhoneNumber.data(), Phone.data(), Phone.size());
        memcpy(RegisterFormInfo.PanNumber.data(), PanNumber.data(), PanNumber.size());
        memcpy((void *)RegisterFormInfo.ContactID.Data(), FormInfo.UniqueId.Data(), FormInfo.UniqueId.GetBytesSize());
        NepBill::Insert(Backend.Database, RegisterFormInfo, true);

        break;
    }
    case NepBill::ContactSubjectType::Billing:
    {
        std::string PaymentTicketUUID =
            Json["paymentTicketUUID"].s();

        std::string BillingIssue =
            Json["billingIssue"].s();
        break;
    }
    default:
        break;
    }

    return true;
}

enum class LoginErrorCode
{
    PhoneInvalid,
    PasswordInvalid,
};

struct LoginReturn
{
    std::string Phone;
    std::string Password;
};

tl::expected<LoginReturn, LoginErrorCode> LoginContact(NepBill::App &Backend, const crow::json::rvalue &Json)
{
    std::string Phone =
        Json["PhoneNumber"].s();
    std::string Password =
        Json["Password"].s();
    std::string Otp =
        Json["Otp"].s();

    if (Phone == "9705216410" && Password == "9705216410")
    {
        LoginReturn ReutrnData;
        ReutrnData.Phone = Phone;
        ReutrnData.Password = Password;
        return ReutrnData;
    }
    return tl::make_unexpected(LoginErrorCode::PhoneInvalid);
}

void Logout(NepBill::App &Backend, const crow::json::rvalue &Json)
{
}

crow::response ApiMe(const crow::request &Req, NepBill::App &Backend)
{
    crow::json::wvalue Result;
    crow::response res(200);

    // 1. Extract the Cookie header
    std::string cookies = Req.get_header_value("Cookie");

    // Simple parsing to find "auth_token="
    // (In production, use a proper HTTP parser)
    size_t pos = cookies.find("auth_token=");
    if (pos == std::string::npos)
    {
        Result["loggedIn"] = false;
        res = crow::response(200);
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());

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
        if (decoded.has_payload_claim("PhoneNumber"))
        {
            Result["PhoneNumber"] = decoded.get_payload_claim("PhoneNumber").as_string();
        }

        Result["loggedIn"] = true;
        Result["Role"] = "Admin";
    }
    catch (...)
    {
        Result["loggedIn"] = false;
        res = crow::response(200);
    }
    // FIX: Set the content type to JSON and dump the JSON object to a string string
    res.add_header("Content-Type", "application/json");
    res.write(Result.dump());

    return res;
}

crow::response AdminContactFormsQuery(const crow::request &Req, NepBill::App &Backend)
{
    crow::response Response(200);
    crow::json::wvalue Result;

    Result["Count"] = 0;
    auto RequestJson =
        crow::json::load(Req.body);

    NepBill::ContactFormQuery ActualFormQuery;

    if (RequestJson["State"].i() != -1)
        ActualFormQuery.State = (NepBill::ContactStates)RequestJson["State"].i();

    ActualFormQuery.Pagination.Limit = RequestJson["limit"].i();

    if (RequestJson["subject"].s() != "None")
        ActualFormQuery.Type = GetType(RequestJson["subject"].s());

    if ((RequestJson.has("phone") && RequestJson["phone"].s().size() > 0) == true)
        ActualFormQuery.PhoneNumber = RequestJson["phone"].s();

    if ((RequestJson.has("After") && RequestJson["After"].s().size() > 0) == true)
        ActualFormQuery.CreatedAfter = ParseHtmlDate(RequestJson, "After");

    if ((RequestJson.has("Before") && RequestJson["Before"].s().size() > 0) == true)
        ActualFormQuery.CreatedBefore = ParseHtmlDate(RequestJson, "Before");

    auto Forms = NepBill::GetContactForms(Backend.Database, ActualFormQuery);
    std::cout << "Forms Size: " <<  Forms.size() << "\n";

    for (int i = 0; i < Forms.size(); i++)
    {
        Result["Forms"][i]["UUID"] = Forms[i].UniqueId.ToString();
        Result["Forms"][i]["PhoneNumber"] = std::string(Forms[i].PhoneNumber.data());
        Result["Forms"][i]["Email"] = std::string(Forms[i].Email.data());
        Result["Forms"][i]["Type"] = GetStrFromType(Forms[i].Type);
        Result["Forms"][i]["CreatedAt"] = FormatDateForHtml(NepBill::ToUnixTime(Forms[i].CreatedAt));
        Result["Forms"][i]["State"] = GetStrFromContactState(Forms[i].State);
        std::cout << Forms[i].Message.data() << "\n"; 
        std::cout << "Email: " << Forms[i].Email.data()  << "\n";
    }
    Result["Count"] = Forms.size();

    std::cout << Forms.size() << "\n";
    // FIX: Set the content type to JSON and dump the JSON object to a string string
    Response.add_header("Content-Type", "application/json");
    Response.write(Result.dump());

    return Response;
}

crow::response AdminContactFormsQueryDetails(const crow::request &Req, NepBill::App &Backend)
{
    crow::response Response(200);
    crow::json::wvalue Result;

    auto RequestJson =
        crow::json::load(Req.body);

    std::string_view UUIDStrView = std::string(RequestJson["uuid"].s());

    NepBill::ContactFormQuery Query;
    Query.UniqueId = NepBill::UUID::FromString(UUIDStrView);

    auto QueryForms = NepBill::GetContactForms(Backend.Database, Query);

    if (QueryForms.size() == 0)
    {
        Result["Valid"] = false;
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        Response.add_header("Content-Type", "application/json");
        Response.write(Result.dump());

        return Response;
    }

    auto Form = QueryForms[0];
 
    Result["PhoneNumber"] = Form.PhoneNumber.data();
    Result["Email"] = Form.Email.data();
    Result["Type"] = GetStrFromType(Form.Type);
    Result["Message"] = Form.Message.data();
    Result["CreatedAt"] = FormatDateForHtml(NepBill::ToUnixTime(Form.CreatedAt));
    Result["State"] = GetStrFromContactState(Form.State);
    Result["Valid"] = true;

    // FIX: Set the content type to JSON and dump the JSON object to a string string
    Response.add_header("Content-Type", "application/json");
    Response.write(Result.dump());

    return Response;
}

crow::response AdminContactFormMarkState(const crow::request &Req, NepBill::App &Backend)
{
    crow::response Response(200);
    crow::json::wvalue Result;

    auto RequestJson =
        crow::json::load(Req.body);

    std::string_view UUIDStrView = std::string(RequestJson["uuid"].s());

    NepBill::ContactFormQuery Query;
    Query.UniqueId = NepBill::UUID::FromString(UUIDStrView);
    auto QueryForms = NepBill::GetContactForms(Backend.Database, Query);

    if (QueryForms.size() == 0)
    {
        Result["Valid"] = false;
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        Response.add_header("Content-Type", "application/json");
        Response.write(Result.dump());

        return Response;
    }

    auto Form = QueryForms[0];

    Form.State = NepBill::ContactStates::Handled;

    NepBill::Update(Backend.Database, Form.Id, Form, true);

    Result["Valid"] = true;
    // FIX: Set the content type to JSON and dump the JSON object to a string string
    Response.add_header("Content-Type", "application/json");
    Response.write(Result.dump());

    return Response;
}

int main(int argc, char const *argv[])
{
    try
    {
        std::string conn_string =
            "host=localhost dbname=nepbill user=anuj password=heroisanuj7";

        NepBill::App Backend(conn_string);
        crow::SimpleApp App;

        if (Backend.Database.is_open())
        {
            std::cout << "Connected to database successfully: " << Backend.Database.dbname() << std::endl;
        }
        else
        {
            std::cerr << "Failed to open database" << std::endl;
            return 1;
        }

        CreateTables(Backend.Database);
        std::cout << "1" << "\n";

        std::vector<std::string> tables;

        pqxx::work tx(Backend.Database);

        auto result = tx.exec(R"(
                SELECT table_name
                FROM information_schema.tables
                WHERE table_schema = 'public'
            )");

        for (const auto &row : result)
        {
            std::cout << row["table_name"].c_str() << "\n";
        }

        tx.commit();
        std::cout << "2" << "\n";

        App.static_file("/styles.css", "templates/static/styles.css");
        App.static_file("/Admin.css", "templates/static/Admin.css");
        App.static_file("/login.html", "templates/pages/login.html");
        App.static_file("/Account/Account.html", "templates/pages/Account/Account.html");
        App.static_file("/Account/Documents.html", "templates/pages/Account/Documents.html");
        App.static_file("/Account/Account.js", "templates/pages/Account/Account.js");
        App.static_file("/Admin/Dashboard.html", "templates/pages/Admin/Dashboard.html");
        App.static_file("/Admin/ContactForms.html", "templates/pages/Admin/ContactForms.html");
        App.static_file("/Admin/ContactForms.js", "templates/pages/Admin/ContactForms.js");
        App.static_file("/Admin/NewForms.html", "templates/pages/Admin/NewForms.html");
        App.static_file("/Admin/FormDetails.html", "templates/pages/Admin/FormDetails.html");
        App.static_file("/Admin/FormDetails.js", "templates/pages/Admin/FormDetails.js");
        App.static_file("/Common/Contact.html", "templates/pages/Common/Contact.html");
        App.static_file("/Common/Contact.js", "templates/pages/Common/Contact.js");
        App.static_file("/logo.jpg", "templates/images/logo.jpg");
        App.static_file("/index.js", "templates/static/index.js");

        CROW_ROUTE(App, "/")
        ([]()
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

    return crow::response(200, content); });

        CROW_ROUTE(App, "/api/send-otp").methods(crow::HTTPMethod::POST)([](const crow::request &Req)
                                                                         {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
            return crow::response(400);

        std::string Phone =
            Json["phone"].s();

        std::string Password =
            Json["password"].s();

        crow::json::wvalue Result;

        Result["message"] =
            "OTP sent successfully";

        return crow::response(Result); });

        CROW_ROUTE(App, "/api/logout").methods(crow::HTTPMethod::POST)([](const crow::request &Req)
                                                                       {
        crow::response res(200);
    
        // Set cookie to expire, effectively clearing it from the browser
        res.add_header("Set-Cookie", 
            // Cookie format: name=value; HttpOnly; Secure; SameSite=Strict; Max-Age=3600
            "auth_token=; HttpOnly; Path=/; Secure; SameSite=Strict; Max-Age=0");
        
        crow::json::wvalue json;
        json["success"] = true;

        res.set_header("Content-Type", "application/json");
        res.write(json.dump());
        return res; });

        CROW_ROUTE(App, "/api/login").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                      {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
            return crow::response(400);

        auto FunctionResult = LoginContact(Backend, Json);

        crow::json::wvalue Result;
        crow::response res(200);

        if(!FunctionResult)
        {
            auto ErrorCode = FunctionResult.error();
            Result["State"] = false;

            if(ErrorCode == LoginErrorCode::PasswordInvalid)
            Result["message"] = "Password Invalid";
            
            if(ErrorCode == LoginErrorCode::PhoneInvalid)
            Result["message"] = "PhoneInvalid";
        }
        else
        {
            std::cout << "VALID ENTRY LOGIN" << "\n";
        // Generate JWT
            auto token = std::string(jwt::create()
                            .set_issuer("NepBill")
                            .set_type("JWS")
                            .set_payload_claim("PhoneNumber", jwt::claim(std::string(FunctionResult.value().Phone)))
                            .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds{MaxLoginCookieAge}) // 1 hour
                            .sign(jwt::algorithm::hs256{JWT_COOKIE_KEY}));
           
            // Set token in HttpOnly Cookie
            // Cookie format: name=value; HttpOnly; Secure; SameSite=Strict; Max-Age=3600
            std::string cookie_header = std::string("auth_token=") + token + 
            "; HttpOnly; Path=/; Secure; SameSite=Strict; Max-Age=" + std::to_string(MaxLoginCookieAge);
            res.add_header("Set-Cookie", cookie_header);

            Result["State"] = true;
            Result["message"] = "Login successful";
        }
        
        // FIX: Set the content type to JSON and dump the JSON object to a string string
        res.add_header("Content-Type", "application/json");
        res.write(Result.dump());

        return res; });

        CROW_ROUTE(App, "/api/SubmitContact").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                              {
        auto Json =
            crow::json::load(Req.body);

        if (!Json)
            return crow::response(400);

        auto FunctionResult = SubmitContact(Backend, Json);

        crow::json::wvalue Result;

        Result["message"] =
            "OTP sent successfully";

        return crow::response(Result); });

        CROW_ROUTE(App, "/api/admin/contactforms/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                         { return AdminContactFormsQuery(Req, Backend); });

        CROW_ROUTE(App, "/api/admin/contactforms/querydetails").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                { return AdminContactFormsQueryDetails(Req, Backend); });

        CROW_ROUTE(App, "/api/admin/contactforms/markState").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                               { return AdminContactFormMarkState(Req, Backend); });

        CROW_ROUTE(App, "/api/me")
        ([&Backend](const crow::request &Req)
         { return ApiMe(Req, Backend); });

        App.port(18080).multithreaded().run();
        Backend.Database.close();
    }
    catch (const std::exception &e)
    {
        // This will print the actual error message!
        std::cerr << "Uncaught exception: " << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "Unknown non-standard exception thrown." << std::endl;
        return -1;
    }

    return 0;
}
