#include <iostream>
#include <string>
#include <strstream>
#include <iosfwd>
#include <sstream>
#include <initializer_list>
#include <utility>

#include "Tables.h"

namespace NepBill
{
    std::string MakeVarchar(uint32_t Length)
    {
        std::ostringstream Stream;
        Stream << "VARCHAR(" << Length << ")";
        return Stream.str();
    }

    std::string BuildCreateQuery(const char *TableName, std::initializer_list<std::pair<const char *, std::string>> Columns)
    {
        std::ostringstream Stream;
        Stream << "CREATE TABLE IF NOT EXISTS " << TableName << " (";

        bool First = true;
        for (const auto &Column : Columns)
        {
            if (!First)
            {
                Stream << ", ";
            }

            Stream << Column.first << " " << Column.second;
            First = false;
        }

        Stream << ")";
        return Stream.str();
    }

    std::string BuildInsertQuery(const char *TableName, std::initializer_list<const char *> Columns)
    {
        std::ostringstream Stream;
        Stream << "INSERT INTO " << TableName << " (";

        bool First = true;
        for (const auto *Column : Columns)
        {
            if (!First)
            {
                Stream << ", ";
            }

            Stream << Column;
            First = false;
        }

        Stream << ") VALUES (";
        First = true;
        size_t Index = 0;
        for (const auto *Column : Columns)
        {
            (void)Column;
            if (!First)
            {
                Stream << ", ";
            }

            Stream << "$" << (++Index);
            First = false;
        }

        Stream << ")";
        return Stream.str();
    }

    std::string BuildCountQuery(const char *TableName)
    {
        std::ostringstream Stream;
        Stream << "SELECT COUNT(*) FROM " << TableName;
        return Stream.str();
    }

    std::string BuildByIdQuery(const char *TableName, std::initializer_list<const char *> Columns)
    {
        std::ostringstream Stream;
        Stream << "SELECT ";

        bool First = true;
        for (const auto *Column : Columns)
        {
            if (!First)
            {
                Stream << ", ";
            }

            Stream << Column;
            First = false;
        }

        Stream << " FROM " << TableName << " WHERE Id = $1";
        return Stream.str();
    }

#pragma region ContactFormInfo

    const char *ContactFormInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ContactFormInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UniqueId", "UUID"},
                {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"Email", MakeVarchar(kEmailLength)},
                {"Message", MakeVarchar(kContactMessageLength)},
                {"Type", "INT"},
                {"CreatedAt", "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"},
                {"State", "INT DEFAULT 0"} // Changed from BOOLEAN DEFAULT FALSE
            });

        return Query.c_str();
    }

    const char *ContactFormInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ContactFormInfo",
            {"UniqueId",
             "PhoneNumber",
             "Email",
             "Message",
             "Type",
             "CreatedAt",
             "State"});

        return Query.c_str();
    }

    const char *ContactFormInfo::GetCountQuery()
    {
        static std::string Query =
            BuildCountQuery("ContactFormInfo");

        return Query.c_str();
    }

    const char *ContactFormInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ContactFormInfo",
            {"Id",
             "UniqueId",
             "PhoneNumber",
             "Email",
             "Message",
             "Type",
             "CreatedAt",
             "State"});

        return Query.c_str();
    }

    const char *ContactFormInfo::GetName()
    {
        return "ContactFormInfo";
    }

#pragma endregion

#pragma region RegisterAccountContactFormInfo

    const char *RegisterAccountContactFormInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "RegisterAccountContactFormInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"ContactID", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
                {"PrimaryPhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"VatNumber", MakeVarchar(kVatNumberLength)},
                {"PanNumber", MakeVarchar(kPanNumberLength)},
                {"Country", MakeVarchar(kCountryNameLength)},
                {"Address", MakeVarchar(kAddressLength)},
            });

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "RegisterAccountContactFormInfo",
            {
                "ContactID",
                "Name",
                "PrimaryPhoneNumber",
                "VatNumber",
                "PanNumber",
                "Country",
                "Address",
            });

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetCountQuery()
    {
        static std::string Query =
            BuildCountQuery("RegisterAccountContactFormInfo");

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "RegisterAccountContactFormInfo",
            {
                "Id",
                "ContactID",
                "Name",
                "PrimaryPhoneNumber",
                "VatNumber",
                "PanNumber",
                "Country",
                "Address",
            });

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetName()
    {
        return "RegisterAccountContactFormInfo";
    }

#pragma endregion

#pragma region ContactFormStateMetaInfo

    const char *ContactFormStateMetaInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ContactFormStateMetaInfo",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"FormID", "UUID"},
             {"StateMessage", MakeVarchar(kContactMessageLength)}});

        return Query.c_str();
    }

    const char *ContactFormStateMetaInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ContactFormStateMetaInfo",
            {"FormID",
             "StateMessage"});

        return Query.c_str();
    }

    const char *ContactFormStateMetaInfo::GetCountQuery()
    {
        static std::string Query =
            BuildCountQuery("ContactFormStateMetaInfo");

        return Query.c_str();
    }

    const char *ContactFormStateMetaInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ContactFormStateMetaInfo",
            {"Id",
             "FormID",
             "StateMessage"});

        return Query.c_str();
    }

    const char *ContactFormStateMetaInfo::GetName()
    {
        return "ContactFormStateMetaInfo";
    }

#pragma endregion

#pragma region AccountRoleInfo

    const char *AccountRoleInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "AccountRoleInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"AccountID", "UUID"},
                {"Role", "INT"} // or "SMALLINT" depending on your schema setup
            });

        return Query.c_str();
    }

    const char *AccountRoleInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "AccountRoleInfo",
            {"AccountID",
             "Role"});

        return Query.c_str();
    }

    const char *AccountRoleInfo::GetCountQuery()
    {
        static std::string Query =
            BuildCountQuery("AccountRoleInfo");

        return Query.c_str();
    }

    const char *AccountRoleInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "AccountRoleInfo",
            {"Id",
             "AccountID",
             "Role"});

        return Query.c_str();
    }

    const char *AccountRoleInfo::GetName()
    {
        return "AccountRoleInfo";
    }

#pragma endregion

#pragma region Business

    const char *Business::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Business",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UniqueId", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
                {"PrimaryPhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"VatNumber", MakeVarchar(kVatNumberLength)},
                {"PanNumber", MakeVarchar(kPanNumberLength)},
                {"Country", MakeVarchar(kCountryNameLength)},
                {"Address", MakeVarchar(kAddressLength)},
                {"ParentID", "UUID"},
                {"CreatedAt", "TIMESTAMP"},
            });
        return Query.c_str();
    }

    const char *Business::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Business",
            {"UniqueId", "Name", "PrimaryPhoneNumber", "VatNumber", "PanNumber", "Country", "Address", "ParentID", "CreatedAt"});
        return Query.c_str();
    }

    const char *Business::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("Business");
        return Query.c_str();
    }

    const char *Business::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "Business",
            {"Id", "UniqueId", "Name", "PrimaryPhoneNumber", "VatNumber", "PanNumber", "Country", "Address", "ParentID", "CreatedAt"});
        return Query.c_str();
    }

    const char *Business::GetName()
    {
        return "Business";
    }

#pragma endregion

#pragma region BusinessSubscription

    const char *BusinessSubscription::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "BusinessSubscription",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UnqiueId", "UUID"},
                {"BusinessID", "UUID"},
                {"Service", "INTEGER"},
                {"StartedAt", "TIMESTAMP"},
                {"ExpiresAt", "TIMESTAMP"},
                {"IsActive", "BOOLEAN"},
            });
        return Query.c_str();
    }

    const char *BusinessSubscription::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "BusinessSubscription",
            {"UnqiueId", "BusinessID", "Service", "StartedAt", "ExpiresAt", "IsActive"});
        return Query.c_str();
    }

    const char *BusinessSubscription::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("BusinessSubscription");
        return Query.c_str();
    }

    const char *BusinessSubscription::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "BusinessSubscription",
            {"Id", "UnqiueId", "BusinessID", "Service", "StartedAt", "ExpiresAt", "IsActive"});
        return Query.c_str();
    }

    const char *BusinessSubscription::GetName()
    {
        return "BusinessSubscription";
    }

#pragma endregion

#pragma region BusinessLogin

    const char *BusinessLogin::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "BusinessLogin",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UnqiueId", "UUID"},
                {"BusinessID", "UUID"},
                {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"PasswordHash", MakeVarchar(kPasswordHashLength)},
                {"Role", "INTEGER"},
                {"IsActive", "BOOLEAN"},
            });
        return Query.c_str();
    }

    const char *BusinessLogin::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "BusinessLogin",
            {"UnqiueId", "BusinessID", "PhoneNumber", "PasswordHash", "Role", "IsActive"});
        return Query.c_str();
    }

    const char *BusinessLogin::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("BusinessLogin");
        return Query.c_str();
    }

    const char *BusinessLogin::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "BusinessLogin",
            {"Id", "UnqiueId", "BusinessID", "PhoneNumber", "PasswordHash", "Role", "IsActive"});
        return Query.c_str();
    }

    const char *BusinessLogin::GetName()
    {
        return "BusinessLogin";
    }

#pragma endregion

#pragma region ItemInvoice

    const char *ItemInvoice::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ItemInvoice",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"SenderId", "UUID"},
                {"RecieverId", "UUID"},
                {"UniqueID", "UUID"},
                {"State", "INTEGER"},
                {"VatPercent", "DOUBLE PRECISION"},
                {"LineTotal", "DOUBLE PRECISION"},
                {"SenderType", "INTEGER"},
                {"RecieverType", "INTEGER"},
                {"CreatedAt", "TIMESTAMP"},
                {"LastPaymentTicketDate", "TIMESTAMP"},
                {"ClosedAt", "TIMESTAMP"},
            });
        return Query.c_str();
    }

    const char *ItemInvoice::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemInvoice",
            {"SenderId", "RecieverId", "UniqueID", "State", "VatPercent", "LineTotal", "SenderType", "RecieverType", "CreatedAt", "LastPaymentTicketDate", "ClosedAt"});
        return Query.c_str();
    }

    const char *ItemInvoice::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("ItemInvoice");
        return Query.c_str();
    }

    const char *ItemInvoice::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ItemInvoice",
            {"Id", "SenderId", "RecieverId", "UniqueID", "State", "VatPercent", "LineTotal", "SenderType", "RecieverType", "CreatedAt", "LastPaymentTicketDate", "ClosedAt"});
        return Query.c_str();
    }

    const char *ItemInvoice::GetName()
    {
        return "ItemInvoice";
    }

#pragma endregion

#pragma region ItemInvoiceLine

    const char *ItemInvoiceLine::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ItemInvoiceLine",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"InvoiceId", "UUID"},
                {"ItemId", "UUID"},
                {"UnqiueId", "UUID"},
                {"OrderedQuantity", "INTEGER"},
                {"DeliveredQuantity", "INTEGER"},
                {"ItemWeight", "DOUBLE PRECISION"},
                {"UnitPrice", "DOUBLE PRECISION"},
                {"UnitDiscountPercet", "DOUBLE PRECISION"},
                {"LineTotal", "DOUBLE PRECISION"},
            });
        return Query.c_str();
    }

    const char *ItemInvoiceLine::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemInvoiceLine",
            {"InvoiceId", "ItemId", "UnqiueId", "OrderedQuantity", "DeliveredQuantity", "ItemWeight", "UnitPrice", "UnitDiscountPercet", "LineTotal"});
        return Query.c_str();
    }

    const char *ItemInvoiceLine::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("ItemInvoiceLine");
        return Query.c_str();
    }

    const char *ItemInvoiceLine::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ItemInvoiceLine",
            {"Id", "InvoiceId", "ItemId", "UnqiueId", "OrderedQuantity", "DeliveredQuantity", "ItemWeight", "UnitPrice", "UnitDiscountPercet", "LineTotal"});
        return Query.c_str();
    }

    const char *ItemInvoiceLine::GetName()
    {
        return "ItemInvoiceLine";
    }

#pragma endregion

#pragma region BookingInvoice

    const char *BookingInvoice::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "BookingInvoice",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"GuestId", "UUID"},
                {"HotelBusinessID", "UUID"},
                {"UniqueID", "UUID"},
                {"State", "INTEGER"},
                {"VatPercent", "DOUBLE PRECISION"},
                {"LineTotal", "DOUBLE PRECISION"},
                {"CreatedAt", "TIMESTAMP"},
                {"LastPaymentTicketDate", "TIMESTAMP"},
                {"ClosedAt", "TIMESTAMP"},
            });
        return Query.c_str();
    }

    const char *BookingInvoice::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "BookingInvoice",
            {"GuestId", "HotelBusinessID", "UniqueID", "State", "VatPercent", "LineTotal", "CreatedAt", "LastPaymentTicketDate", "ClosedAt"});
        return Query.c_str();
    }

    const char *BookingInvoice::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("BookingInvoice");
        return Query.c_str();
    }

    const char *BookingInvoice::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "BookingInvoice",
            {"Id", "GuestId", "HotelBusinessID", "UniqueID", "State", "VatPercent", "LineTotal", "CreatedAt", "LastPaymentTicketDate", "ClosedAt"});
        return Query.c_str();
    }

    const char *BookingInvoice::GetName()
    {
        return "BookingInvoice";
    }

#pragma endregion

#pragma region BookingInvoiceLine

    const char *BookingInvoiceLine::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "BookingInvoiceLine",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"InvoiceId", "UUID"},
                {"UnqiueId", "UUID"},
                {"BookingFeatureId", "UUID"},
                {"Count", "DOUBLE PRECISION"},
                {"LineTotal", "DOUBLE PRECISION"},
            });
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "BookingInvoiceLine",
            {"InvoiceId", "UnqiueId", "BookingFeatureId", "Count", "LineTotal"});
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("BookingInvoiceLine");
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "BookingInvoiceLine",
            {"Id", "InvoiceId", "UnqiueId", "BookingFeatureId", "Count", "LineTotal"});
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetName()
    {
        return "BookingInvoiceLine";
    }

#pragma endregion

#pragma region PaymentTicket

    const char *PaymentTicket::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "PaymentTicket",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"PaidAmount", "DOUBLE PRECISION"},
                {"SenderType", "INTEGER"},
                {"RecieverType", "INTEGER"},
                {"InvoiceID", "UUID"},
                {"UniqueID", "UUID"},
                {"PaidType", "INTEGER"},
                {"CreatedAt", "TIMESTAMP"},
                {"Note", MakeVarchar(kPaymentTicketNotesLength)},
            });
        return Query.c_str();
    }

    const char *PaymentTicket::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "PaymentTicket",
            {"PaidAmount", "SenderType", "RecieverType", "InvoiceID", "UniqueID", "PaidType", "CreatedAt", "Note"});
        return Query.c_str();
    }

    const char *PaymentTicket::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("PaymentTicket");
        return Query.c_str();
    }

    const char *PaymentTicket::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "PaymentTicket",
            {"Id", "PaidAmount", "SenderType", "RecieverType", "InvoiceID", "UniqueID", "PaidType", "CreatedAt", "Note"});
        return Query.c_str();
    }

    const char *PaymentTicket::GetName()
    {
        return "PaymentTicket";
    }

#pragma endregion

#pragma region ItemCategory

    const char *ItemCategory::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ItemCategory",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"UniqueID", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
            });
        return Query.c_str();
    }

    const char *ItemCategory::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemCategory",
            {"BusinessID", "UniqueID", "Name"});
        return Query.c_str();
    }

    const char *ItemCategory::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("ItemCategory");
        return Query.c_str();
    }

    const char *ItemCategory::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ItemCategory",
            {"Id", "BusinessID", "UniqueID", "Name"});
        return Query.c_str();
    }

    const char *ItemCategory::GetName()
    {
        return "ItemCategory";
    }

#pragma endregion

#pragma region Item

    const char *Item::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Item",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"CategoryId", "UUID"},
                {"UniqueID", "UUID"},
                {"Name", MakeVarchar(ItemNameLength)},
                {"LowStockThresold", "INTEGER"},
                {"CostPrice", "DOUBLE PRECISION"},
                {"SalesPrice", "DOUBLE PRECISION"},
                {"DiscountPercent", "DOUBLE PRECISION"},
                {"Description", MakeVarchar(ItemDescriptionLength)},
            });
        return Query.c_str();
    }

    const char *Item::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Item",
            {"BusinessID", "CategoryId", "UniqueID", "Name", "LowStockThresold", "CostPrice", "SalesPrice", "DiscountPercent", "Description"});
        return Query.c_str();
    }

    const char *Item::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("Item");
        return Query.c_str();
    }

    const char *Item::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "Item",
            {"Id", "BusinessID", "CategoryId", "UniqueID", "Name", "LowStockThresold", "CostPrice", "SalesPrice", "DiscountPercent", "Description"});
        return Query.c_str();
    }

    const char *Item::GetName()
    {
        return "Item";
    }

#pragma endregion

#pragma region ItemStockLedger

    const char *ItemStockLedger::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ItemStockLedger",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"ItemId", "UUID"},
                {"StockDelta", "INTEGER"},
                {"CreatedAt", "TIMESTAMP"},
            });
        return Query.c_str();
    }

    const char *ItemStockLedger::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemStockLedger",
            {"BusinessID", "ItemId", "StockDelta", "CreatedAt"});
        return Query.c_str();
    }

    const char *ItemStockLedger::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("ItemStockLedger");
        return Query.c_str();
    }

    const char *ItemStockLedger::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ItemStockLedger",
            {"Id", "BusinessID", "ItemId", "StockDelta", "CreatedAt"});
        return Query.c_str();
    }

    const char *ItemStockLedger::GetName()
    {
        return "ItemStockLedger";
    }

#pragma endregion

#pragma region Suppliers

    const char *Suppliers::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Suppliers",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"UnqiueId", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
                {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"PanNumber", MakeVarchar(kPanNumberLength)},
                {"OpeningBalance", "DOUBLE PRECISION"},
            });
        return Query.c_str();
    }

    const char *Suppliers::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Suppliers",
            {"BusinessID", "UnqiueId", "Name", "PhoneNumber", "PanNumber", "OpeningBalance"});
        return Query.c_str();
    }

    const char *Suppliers::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("Suppliers");
        return Query.c_str();
    }

    const char *Suppliers::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "Suppliers",
            {"Id", "BusinessID", "UnqiueId", "Name", "PhoneNumber", "PanNumber", "OpeningBalance"});
        return Query.c_str();
    }

    const char *Suppliers::GetName()
    {
        return "Suppliers";
    }

#pragma endregion

#pragma region SuppliersItemTable

    const char *SuppliersItemTable::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "SuppliersItemTable",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"SupplierID", "UUID"},
                {"ItemID", "UUID"},
            });
        return Query.c_str();
    }

    const char *SuppliersItemTable::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "SuppliersItemTable",
            {"BusinessID", "SupplierID", "ItemID"});
        return Query.c_str();
    }

    const char *SuppliersItemTable::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("SuppliersItemTable");
        return Query.c_str();
    }

    const char *SuppliersItemTable::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "SuppliersItemTable",
            {"Id", "BusinessID", "SupplierID", "ItemID"});
        return Query.c_str();
    }

    const char *SuppliersItemTable::GetName()
    {
        return "SuppliersItemTable";
    }

#pragma endregion

#pragma region RoomInfo

    const char *RoomInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "RoomInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"Name", MakeVarchar(kNameLength)},
                {"BasePrice", "DOUBLE PRECISION"},
                {"BedCount", "INTEGER"},
                {"Description", MakeVarchar(RoomDescriptionLength)},
                {"UniqueID", "UUID"},
                {"State", "INTEGER"},
                {"BusinessID", "UUID"},
            });
        return Query.c_str();
    }

    const char *RoomInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "RoomInfo",
            {"Name", "BasePrice", "BedCount", "Description", "UniqueID", "State", "BusinessID"});
        return Query.c_str();
    }

    const char *RoomInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("RoomInfo");
        return Query.c_str();
    }

    const char *RoomInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "RoomInfo",
            {"Id", "Name", "BasePrice", "BedCount", "Description", "UniqueID", "State", "BusinessID"});
        return Query.c_str();
    }

    const char *RoomInfo::GetName()
    {
        return "RoomInfo";
    }

#pragma endregion

#pragma region RoomFacilityInfo

    const char *RoomFacilityInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "RoomFacilityInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UniqueID", "UUID"},
                {"RoomID", "UUID"},
                {"BusinessID", "UUID"},
                {"FacilityInfo", MakeVarchar(RoomFacilityLength)},
                {"FacilityPrice", "DOUBLE PRECISION"},
            });
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "RoomFacilityInfo",
            {"UniqueID", "RoomID", "BusinessID", "FacilityInfo", "FacilityPrice"});
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("RoomFacilityInfo");
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "RoomFacilityInfo",
            {"Id", "UniqueID", "RoomID", "BusinessID", "FacilityInfo", "FacilityPrice"});
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetName()
    {
        return "RoomFacilityInfo";
    }

#pragma endregion

#pragma region GuestInfo

    const char *GuestInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "GuestInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"UniqueID", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
                {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"Country", MakeVarchar(kCountryNameLength)},
            });
        return Query.c_str();
    }

    const char *GuestInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "GuestInfo",
            {"BusinessID", "UniqueID", "Name", "PhoneNumber", "Country"});
        return Query.c_str();
    }

    const char *GuestInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("GuestInfo");
        return Query.c_str();
    }

    const char *GuestInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "GuestInfo",
            {"Id", "BusinessID", "UniqueID", "Name", "PhoneNumber", "Country"});
        return Query.c_str();
    }

    const char *GuestInfo::GetName()
    {
        return "GuestInfo";
    }

#pragma endregion

#pragma region HotelBookingInfo

    const char *HotelBookingInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "HotelBookingInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"UniqueID", "UUID"},
                {"InvoiceID", "UUID"},
                {"RoomID", "UUID"},
                {"HotelBusinessID", "UUID"},
                {"CreatedAt", "TIMESTAMP"},
                {"CheckIn", "TIMESTAMP"},
                {"CheckOut", "TIMESTAMP"},
            });
        return Query.c_str();
    }

    const char *HotelBookingInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "HotelBookingInfo",
            {"UniqueID", "InvoiceID", "RoomID", "HotelBusinessID", "CreatedAt", "CheckIn", "CheckOut"});
        return Query.c_str();
    }

    const char *HotelBookingInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("HotelBookingInfo");
        return Query.c_str();
    }

    const char *HotelBookingInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "HotelBookingInfo",
            {"Id", "UniqueID", "InvoiceID", "RoomID", "HotelBusinessID", "CreatedAt", "CheckIn", "CheckOut"});
        return Query.c_str();
    }

    const char *HotelBookingInfo::GetName()
    {
        return "HotelBookingInfo";
    }

#pragma endregion

#pragma region HotelBookingGuestTable

    const char *HotelBookingGuestTable::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "HotelBookingGuestTable",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"HotelBusinessID", "UUID"},
                {"GuestID", "UUID"},
                {"BookingID", "UUID"},
            });
        return Query.c_str();
    }

    const char *HotelBookingGuestTable::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "HotelBookingGuestTable",
            {"HotelBusinessID", "GuestID", "BookingID"});
        return Query.c_str();
    }

    const char *HotelBookingGuestTable::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("HotelBookingGuestTable");
        return Query.c_str();
    }

    const char *HotelBookingGuestTable::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "HotelBookingGuestTable",
            {"Id", "HotelBusinessID", "GuestID", "BookingID"});
        return Query.c_str();
    }

    const char *HotelBookingGuestTable::GetName()
    {
        return "HotelBookingGuestTable";
    }

#pragma endregion

#pragma region HotelBookingAdditionalCostInfo

    const char *HotelBookingAdditionalCostInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "HotelBookingAdditionalCostInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"HotelBusinessID", "UUID"},
                {"UnqiueID", "UUID"},
                {"BookingID", "UUID"},
                {"ItemID", "UUID"},
                {"PricePerUnit", "DOUBLE PRECISION"},
                {"UnitCount", "INTEGER"},
            });
        return Query.c_str();
    }

    const char *HotelBookingAdditionalCostInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "HotelBookingAdditionalCostInfo",
            {"HotelBusinessID", "UnqiueID", "BookingID", "ItemID", "PricePerUnit", "UnitCount"});
        return Query.c_str();
    }

    const char *HotelBookingAdditionalCostInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("HotelBookingAdditionalCostInfo");
        return Query.c_str();
    }

    const char *HotelBookingAdditionalCostInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "HotelBookingAdditionalCostInfo",
            {"Id", "HotelBusinessID", "UnqiueID", "BookingID", "ItemID", "PricePerUnit", "UnitCount"});
        return Query.c_str();
    }

    const char *HotelBookingAdditionalCostInfo::GetName()
    {
        return "HotelBookingAdditionalCostInfo";
    }

#pragma endregion

#pragma region StaffInfo

    const char *StaffInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "StaffInfo",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"UnqiueID", "UUID"},
                {"Name", MakeVarchar(kNameLength)},
                {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"EmergencyPhoneNumber", MakeVarchar(kPhoneNumberLength)},
                {"IsActive", "BOOLEAN"},
                {"SalaryPerDay", "DOUBLE PRECISION"},
            });
        return Query.c_str();
    }

    const char *StaffInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "StaffInfo",
            {"BusinessID", "UnqiueID", "Name", "PhoneNumber", "EmergencyPhoneNumber", "IsActive", "SalaryPerDay"});
        return Query.c_str();
    }

    const char *StaffInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("StaffInfo");
        return Query.c_str();
    }

    const char *StaffInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "StaffInfo",
            {"Id", "BusinessID", "UnqiueID", "Name", "PhoneNumber", "EmergencyPhoneNumber", "IsActive", "SalaryPerDay"});
        return Query.c_str();
    }

    const char *StaffInfo::GetName()
    {
        return "StaffInfo";
    }

#pragma endregion

#pragma region StaffLedger

    const char *StaffLedger::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "StaffLedger",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"StaffID", "UUID"},
                {"CheckIn", "TIMESTAMP"},
                {"CheckOut", "TIMESTAMP"},
                {"PaymentRelated", "BOOLEAN"},
                {"SalaryPaymentID", "UUID"},
            });
        return Query.c_str();
    }

    const char *StaffLedger::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "StaffLedger",
            {"BusinessID", "StaffID", "CheckIn", "CheckOut", "PaymentRelated", "SalaryPaymentID"});
        return Query.c_str();
    }

    const char *StaffLedger::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("StaffLedger");
        return Query.c_str();
    }

    const char *StaffLedger::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "StaffLedger",
            {"Id", "BusinessID", "StaffID", "CheckIn", "CheckOut", "PaymentRelated", "SalaryPaymentID"});
        return Query.c_str();
    }

    const char *StaffLedger::GetName()
    {
        return "StaffLedger";
    }

#pragma endregion

#pragma region StaffSalaryPayment

    const char *StaffSalaryPayment::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "StaffSalaryPayment",
            {
                {"Id", "SERIAL PRIMARY KEY"},
                {"BusinessID", "UUID"},
                {"UnqiueID", "UUID"},
                {"StaffID", "UUID"},
                {"Amount", "DOUBLE PRECISION"},
                {"Date", "TIMESTAMP"},
                {"Description", MakeVarchar(SalaryPaymentDescriptionLength)},
            });
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "StaffSalaryPayment",
            {"BusinessID", "UnqiueID", "StaffID", "Amount", "Date", "Description"});
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("StaffSalaryPayment");
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "StaffSalaryPayment",
            {"Id", "BusinessID", "UnqiueID", "StaffID", "Amount", "Date", "Description"});
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetName()
    {
        return "StaffSalaryPayment";
    }

#pragma endregion

}
