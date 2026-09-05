#include "PCH.h"
#include "NepalDateConverter.h"
#include "Maths.h"
#include "NepBill.h"
#include "Routers.h"
#include "jwt-cpp/jwt.h"
#include "tl/expected.hpp"

void CreateTables(pqxx::connection &Db)
{
    // Start a read-write transaction block to execute DDL statements
    pqxx::work tx(Db);

    tx.exec(NepBill::ContactFormInfo::GetCreateQuery());
    tx.exec(NepBill::ContactFormStateMetaInfo::GetCreateQuery());
    tx.exec(NepBill::RegisterAccountContactFormInfo::GetCreateQuery());
    tx.exec(NepBill::AccountRoleInfo::GetCreateQuery());
    tx.exec(NepBill::PdfDocument::GetCreateQuery());
    tx.exec(NepBill::Image::GetCreateQuery());
    tx.exec(NepBill::Business::GetCreateQuery());
    tx.exec(NepBill::Account::GetCreateQuery());

    // Inventory Related
    tx.exec(NepBill::Item::GetCreateQuery());
    tx.exec(NepBill::ItemStockLedger::GetCreateQuery());
    tx.exec(NepBill::ItemCategory::GetCreateQuery());
    tx.exec(NepBill::Suppliers::GetCreateQuery());
    // Inventory Finance Related
    tx.exec(NepBill::ItemInvoice::GetCreateQuery());
    tx.exec(NepBill::ItemInvoiceLine::GetCreateQuery());
    tx.exec(NepBill::PurchaseOrder::GetCreateQuery());
    tx.exec(NepBill::PurchaseOrderLine::GetCreateQuery());

    tx.commit();
    std::cout << "Created Tables\n";
}

int main(int argc, char const *argv[])
{
    try
    {
        std::string conn_string =
            "host=localhost dbname=nepbill user=anuj password=1019";

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

        App.static_file("/styles.css", "templates/static/Styles/styles.css");
        App.static_file("/login.css", "templates/static/Styles/login.css");
        App.static_file("/ContactForms.css", "templates/static/Styles/Admin/ContactForms.css");
        App.static_file("/Specific/BusinessOwner.css", "templates/static/Styles/Specific/BusinessOwner.css");

        App.static_file("/ContactUs.html", "templates/pages/Common/ContactUs.html");
        App.static_file("/Account/Account.html", "templates/pages/Account/Account.html");
        App.static_file("/Login.html", "templates/pages/Common/Login.html");
        App.static_file("/Admin/ContactForms.html", "templates/pages/Account/Admin/ContactForms.html");
        App.static_file("/Admin/Dashboard.html", "templates/pages/Account/Admin/Dashboard.html");
        App.static_file("/Admin/ContactFormDetail.html", "templates/pages/Account/Admin/ContactFormDetail.html");
        App.static_file("/Account/Components/SideBar.html", "templates/pages/Account/Components/SideBar.html");
        App.static_file("/Account/Components/Admin/ContactFormTypeFunctions.html", "templates/pages/Account/Components/Admin/ContactFormTypeFunctions.html");

        App.static_file("/Account/BusinessOwner/Overview.html", "templates/pages/Account/BusinessOwner/Overview.html");
        App.static_file("/Account/BusinessOwner/Profile_Settings.html", "templates/pages/Account/BusinessOwner/Profile_Settings.html");
        App.static_file("/Account/BusinessOwner/Security.html", "templates/pages/Account/BusinessOwner/Security.html");
        App.static_file("/Account/BusinessOwner/Business.html", "templates/pages/Account/BusinessOwner/Business.html");
        App.static_file("/Account/BusinessOwner/Branches_Locations.html", "templates/pages/Account/BusinessOwner/Branches_Locations.html");
        App.static_file("/Account/BusinessOwner/Subscription_Plans.html", "templates/pages/Account/BusinessOwner/Subscription_Plans.html");
        App.static_file("/Account/BusinessOwner/Business_Documents.html", "templates/pages/Account/BusinessOwner/Business_Documents.html");
        App.static_file("/Account/BusinessOwner/Accounts_Roles.html", "templates/pages/Account/BusinessOwner/Accounts_Roles.html");
        App.static_file("/Account/BusinessOwner/Salary_Overview.html", "templates/pages/Account/BusinessOwner/Salary_Overview.html ");
        App.static_file("/Account/BusinessOwner/Revenue_Report.html", "templates/pages/Account/BusinessOwner/Revenue_Report.html");
        App.static_file("/Account/BusinessOwner/Expense_Report.html", "templates/pages/Account/BusinessOwner/Expense_Report.html");
        App.static_file("/Account/BusinessOwner/VAT_Summary.html", "templates/pages/Account/BusinessOwner/VAT_Summary.html");
        App.static_file("/Account/BusinessOwner/Supplier_Payables.html", "templates/pages/Account/BusinessOwner/Supplier_Payables.html");
        App.static_file("/Account/BusinessOwner/Hotel.html", "templates/pages/Account/BusinessOwner/Hotel.html");
        App.static_file("/Account/BusinessOwner/Restaurant.html", "templates/pages/Account/BusinessOwner/Restaurant.html");
        App.static_file("/Account/BusinessOwner/Hostel.html", "templates/pages/Account/BusinessOwner/Hostel.html");

        App.static_file("/Account/Finance/Subscription.html", "templates/pages/Account/Finance/Subscription.html");
        App.static_file("/Account/Finance/Subscription.js", "templates/static/scripts/Finance/Subscription.js");

        App.static_file("/Account/Inventory/Inventory.html", "templates/pages/Account/Inventory/Inventory.html");
        App.static_file("/Account/Inventory/PurchaseOrder.html", "templates/pages/Account/Inventory/PurchaseOrder.html");
        App.static_file("/Account/Inventory/CreatePurchaseOrder.html", "templates/pages/Account/Inventory/CreatePurchaseOrder.html");
        App.static_file("/Inventory/Inventory.js", "templates/static/scripts/Inventory/Inventory.js");
        App.static_file("/Account/Inventory/Stock.html", "templates/pages/Account/Inventory/Stock.html");
        App.static_file("/Account/Inventory/AddItem.html", "templates/pages/Account/Inventory/AddItem.html");
        App.static_file("/Inventory/Stock.js", "templates/static/scripts/Inventory/Stock.js");

        App.static_file("/Account/Supplier/Supplier.html", "templates/pages/Account/Supplier/Supplier.html");
        App.static_file("/Supplier/Supplier.js", "templates/static/scripts/Supplier/Supplier.js");
        App.static_file("/Inventory/PurchaseOrder.js", "templates/static/scripts/Inventory/PuirchaseOrder.js");
        App.static_file("/Inventory/CreatePurchaseOrder.js", "templates/static/scripts/Inventory/CreatePurchaseOrder.js");
        App.static_file("/Inventory/AddItem.js", "templates/static/scripts/Inventory/AddItem.js");

        App.static_file("/Login.js", "templates/static/scripts/Login.js");
        App.static_file("/Admin/ContactEnums.js", "templates/static/scripts/Admin/ContactEnums.js");
        App.static_file("/AccountTypes.js", "templates/static/scripts/AccountTypes.js");
        App.static_file("/SideBar.js", "templates/static/scripts/SideBar.js");
        App.static_file("/index.js", "templates/static/scripts/index.js");
        App.static_file("/ContactUs.js", "templates/static/scripts/ContactUs.js");
        App.static_file("/Account.js", "templates/static/scripts/Account.js");
        App.static_file("/Admin/Dashboard.js", "templates/static/scripts/Admin/Dashboard.js");
        App.static_file("/Admin/ContactForms.js", "templates/static/scripts/Admin/ContactForms.js");
        App.static_file("/Admin/ContactFormDetails.js", "templates/static/scripts/Admin/ContactFormDetails.js");

        NepBill::ContactFormQuery AdminFromQuery;
        AdminFromQuery.PhoneNumber = "9705216410";
        AdminFromQuery.Type = NepBill::ContactSubjectType::RegisterAccount;
        AdminFromQuery.Email = "prianuj7@gmail.com";

        auto QueryAdminForm = NepBill::GetContactForms(Backend.Database, AdminFromQuery);
        std::cout << QueryAdminForm.size() << "\n";

        if (QueryAdminForm.size() == 0)
        {
            NepBill::ContactFormInfo AdminForm;
            memcpy(AdminForm.PhoneNumber.data(), "9705216410", sizeof("9705216410"));
            memcpy(AdminForm.Email.data(), "prianuj7@gmail.com", sizeof("prianuj7@gmail.com"));
            AdminForm.Type = NepBill::ContactSubjectType::RegisterAccount;

            NepBill::RegisterAccountContactFormInfo AdminRegisterAccountInfo;
            AdminRegisterAccountInfo.ContactID = AdminForm.UniqueId;
            memcpy(AdminRegisterAccountInfo.Name.data(), "NepBill", sizeof("NepBill"));
            memcpy(AdminRegisterAccountInfo.Ownername.data(), "Anuj Bhattarai", sizeof("Anuj Bhattarai"));
            memcpy(AdminRegisterAccountInfo.PrimaryPhoneNumber.data(), "9705126410", sizeof("9705126410"));
            memcpy(AdminRegisterAccountInfo.VatNumber.data(), "0", sizeof("0"));
            memcpy(AdminRegisterAccountInfo.VatNumber.data(), "0", sizeof("00"));
            memcpy(AdminRegisterAccountInfo.Country.data(), "Nepal", sizeof("Nepal"));
            memcpy(AdminRegisterAccountInfo.Address.data(), "Kathmandu", sizeof("Kathmandu"));
            memcpy(AdminRegisterAccountInfo.Password.data(), "9705216410", sizeof("9705216410"));

            AdminRegisterAccountInfo.PanPdf = NepBill::UUID::InvalidUUID();
            AdminRegisterAccountInfo.BusinessFrontImage = NepBill::UUID::InvalidUUID();
            AdminRegisterAccountInfo.VatCertificatePdf = NepBill::UUID::InvalidUUID();

            NepBill::EncryptionManager EncryptionManager;

            NepBill::Business AdminBusiness;
            AdminBusiness.RegisterAccountFormId = AdminRegisterAccountInfo.ContactID;

            NepBill::Account AdminAccount;
            AdminAccount.BusinessID = AdminBusiness.UniqueId;
            AdminAccount.ContactFormID = AdminRegisterAccountInfo.ContactID;
            AdminAccount.Role = NepBill::AccountRoles::Admin;
            EncryptionManager.createPasswordHash("9705216410", AdminAccount.PasswordHash);

            NepBill::Insert(Backend.Database, AdminForm);
            NepBill::Insert(Backend.Database, AdminRegisterAccountInfo);
            NepBill::Insert(Backend.Database, AdminBusiness);
            NepBill::Insert(Backend.Database, AdminAccount);

            std::cout << "Admin Account now exists!\n";
        }
        else
        {
            std::cout << "Admin Account already exists!\n";
        }
        CROW_ROUTE(App, "/")([&Backend](const crow::request &Req)
                             { return NepBill::IndexDeafult(Backend, Req); });

        CROW_ROUTE(App, "/api/checklogin").methods(crow::HTTPMethod::GET)([&Backend](const crow::request &Req)
                                                                          { return NepBill::LoggedIn(Backend, Req); });

        CROW_ROUTE(App, "/api/login").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                      { return NepBill::ApiLogin(Backend, Req); });

        CROW_ROUTE(App, "/api/account/queryaccount").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                     { return NepBill::AccountQueryInfo(Backend, Req); });

        CROW_ROUTE(App, "/api/account/logout").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                               { return NepBill::AccountLogout(Req); });

        CROW_ROUTE(App, "/api/contactform/submit").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                   { return NepBill::SubmitContactForm(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/contactform/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::AdminContactFormsQuery(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/contactform/querydetails").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                               { return NepBill::AdminContactFormQueryDetail(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/contactform/editsave").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                           { return NepBill::AdminContactFormEditSave(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/contactform/registeraction/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                       { return NepBill::AdminContactFormRegisterQuery(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/contactform/registeraction/approve").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                         { return NepBill::AdminContactFormRegisterApprove(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventory/itemcateogrybyname/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                         { return NepBill::QueryItemCategoryName(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventory/itemcateogrybyname/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                         { return NepBill::AddItemCategory(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/itemstockunit/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                         { return NepBill::QueryItemStockUnit(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventory/itembyname/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::QueryItemName(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventory/itembyname/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::AddItem(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventory/item/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::QueryItemStock(Backend, Req); });

        CROW_ROUTE(App, "/api/business/supplier/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::AddSuppliers(Backend, Req); });

        CROW_ROUTE(App, "/api/business/supplier/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::QuerySuppliers(Backend, Req); });

        CROW_ROUTE(App, "/api/admin/inventorypurchaseorder/suppliersbyname/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                 { return NepBill::QuerySuppliersByNameFilter(Backend, Req); });


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
