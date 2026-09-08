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
    tx.exec(NepBill::PurchaseOrder::GetCreateQuery());
    tx.exec(NepBill::PurchaseOrderLine::GetCreateQuery());
    
    tx.exec(NepBill::RoomInfo::GetCreateQuery());

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

        App.static_file("/NewStyles.css", "templates/static/Styles/NewStyles.css");
        App.static_file("/styles.css", "templates/static/Styles/styles.css");
        App.static_file("/login.css", "templates/static/Styles/login.css");
        App.static_file("/ContactForms.css", "templates/static/Styles/Admin/ContactForms.css");
        App.static_file("/Specific/BusinessOwner.css", "templates/static/Styles/Specific/BusinessOwner.css");

        App.static_file("/ContactUs.html", "templates/pages/Common/ContactUs.html");
        App.static_file("/Account/Account.html", "templates/pages/Account/Account.html");
        App.static_file("/Login.html", "templates/pages/Common/Login.html");
        App.static_file("/Admin/ContactForms.html", "templates/pages/Account/Admin/ContactForms.html");
        App.static_file("/Admin/ContactForms.js", "templates/static/scripts/Account/Admin/ContactForms.js");

        App.static_file("/Admin/Dashboard.html", "templates/pages/Account/Admin/Dashboard.html");
        App.static_file("/Admin/Dashboard.js", "templates/static/scripts/Account/Admin/Dashboard.js");

        App.static_file("/Admin/ContactFormDetail.html", "templates/pages/Account/Admin/ContactFormDetail.html");
        App.static_file("/Admin/ContactFormDetail.js", "templates/static/scripts/Account/Admin/ContactFormDetail.js");

        App.static_file("/Account/Components/SideBar.html", "templates/pages/Account/Components/SideBar.html");
        App.static_file("/Account/Components/Admin/ContactFormTypeFunctions.html", "templates/pages/Account/Components/Admin/ContactFormTypeFunctions.html");

        App.static_file("/Account/Inventory/AddItem.html", "templates/pages/Account/Components/Inventory/AddItem.html");
        App.static_file("/Account/Inventory/ItemDetail.html", "templates/pages/Account/Components/Inventory/ItemDetail.html");
        App.static_file("/Account/Inventory/Inventory.html", "templates/pages/Account/Components/Inventory/Inventory.html");
        App.static_file("/Account/Inventory/PurchaseOrder.html", "templates/pages/Account/Components/Inventory/PurchaseOrder.html");
        App.static_file("/Account/Inventory/PurchaseOrderDetail.html", "templates/pages/Account/Components/Inventory/PurchaseOrderDetail.html");
        App.static_file("/Account/Inventory/CreatePurchaseOrder.html", "templates/pages/Account/Components/Inventory/CreatePurchaseOrder.html");
        App.static_file("/Account/Inventory/Stock.html", "templates/pages/Account/Components/Inventory/Stock.html");
        App.static_file("/Account/Inventory/Supplier.html", "templates/pages/Account/Components/Inventory/Supplier.html");
        App.static_file("/Account/Inventory/SupplierDetail.html", "templates/pages/Account/Components/Inventory/SupplierDetail.html");
        App.static_file("/Account/Inventory/ChangePurchaseOrderState.html", "templates/pages/Account/Components/Inventory/ChangePurchaseOrderState.html");

        App.static_file("/Account/Rooms/Rooms.html", "templates/pages/Account/Components/Rooms/Rooms.html");
        App.static_file("/Account/Rooms/Rooms.js", "templates/static/scripts/Account/Rooms/Rooms.js");
        App.static_file("/Account/Rooms/AddRoom.html", "templates/pages/Account/Components/Rooms/AddRoom.html");
        App.static_file("/Account/Rooms/AddRoom.js", "templates/static/scripts/Account/Rooms/AddRoom.js");

        App.static_file("/Account/Inventory/AddItem.js", "templates/static/scripts/Account/Inventory/AddItem.js");
        App.static_file("/Account/Inventory/Supplier.js", "templates/static/scripts/Account/Inventory/Supplier.js");
        App.static_file("/Account/Inventory/SupplierDetail.js", "templates/static/scripts/Account/Inventory/SupplierDetail.js");
        App.static_file("/Account/Inventory/ItemDetail.js", "templates/static/scripts/Account/Inventory/ItemDetail.js");
        App.static_file("/Account/Inventory/Inventory.js", "templates/static/scripts/Account/Inventory/Inventory.js");
        App.static_file("/Account/Inventory/PurchaseOrder.js", "templates/static/scripts/Account/Inventory/PurchaseOrder.js");
        App.static_file("/Account/Inventory/PurchaseOrderDetail.js", "templates/static/scripts/Account/Inventory/PurchaseOrderDetail.js");
        App.static_file("/Account/Inventory/CreatePurchaseOrder.js", "templates/static/scripts/Account/Inventory/CreatePurchaseOrder.js");
        App.static_file("/Account/Inventory/Stock.js", "templates/static/scripts/Account/Inventory/Stock.js");
        App.static_file("/Account/Inventory/ChangePurchaseOrderState.js", "templates/static/scripts/Account/Inventory/ChangePurchaseOrderState.js");
        App.static_file("/Account/Inventory/PurchaseOrderEnum.js", "templates/static/scripts/Account/Inventory/PurchaseOrderEnum.js");
        App.static_file("/Account/Inventory/BasicQueries.js", "templates/static/scripts/Account/Inventory/BasicQueries.js");

        App.static_file("/Login.js", "templates/static/scripts/Login.js");
        App.static_file("/ErrorHandler.js", "templates/static/scripts/ErrorHandler.js");
        App.static_file("/Admin/ContactEnums.js", "templates/static/scripts/Account/Admin/ContactEnums.js");
        App.static_file("/AccountTypes.js", "templates/static/scripts/Account/AccountTypes.js");
        App.static_file("/SideBar.js", "templates/static/scripts/Account/SideBar.js");
        App.static_file("/index.js", "templates/static/scripts/index.js");
        App.static_file("/ContactUs.js", "templates/static/scripts/ContactUs.js");
        App.static_file("/Account.js", "templates/static/scripts/Account/Account.js");

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

        CROW_ROUTE(App, "/api/business/inventory/itemcategory/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                    { return NepBill::AddItemCategory(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/itemcategory/query/byname").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                                             { return NepBill::QueryItemCategoryByName(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/item/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                            { return NepBill::AddItem(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/item/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryItemStock(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/itemstockunit/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryItemStockUnit(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/suppliers/query/byname").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QuerySuppliersByNameFilter(Backend, Req); });

        CROW_ROUTE(App, "/api/business/supplier/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::AddSuppliers(Backend, Req); });
                                                                                        
        CROW_ROUTE(App, "/api/business/supplier/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QuerySuppliers(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/item/query/byname").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryItemByName(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/itemdetail/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryItemDetail(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/purchaseorder/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::AddPurchaseOrder(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/purchaseorderlist/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryPurchaseOrderList(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/purchaseorderdetail/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryPurchaseOrderDetail(Backend, Req); });

        CROW_ROUTE(App, "/api/business/inventory/purchaseorder/changeorderstate").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::ChangeOrderState(Backend, Req); });

        CROW_ROUTE(App, "/api/business/room/room/add").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::AddRoomInfo(Backend, Req); });

        CROW_ROUTE(App, "/api/business/room/roomlist/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryRoomList(Backend, Req); });

        CROW_ROUTE(App, "/api/business/room/roomdetail/query").methods(crow::HTTPMethod::POST)([&Backend](const crow::request &Req)
                                                                                        { return NepBill::QueryRoomInfoDetail(Backend, Req); });

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
