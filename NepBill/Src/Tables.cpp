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

#pragma region RegisterAccountContactFormInfo

    const char *RegisterAccountContactFormInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "RegisterAccountContactFormInfo",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"ContactID", "UUID"},
             {"Name", MakeVarchar(kNameLength)},
             {"Ownername", MakeVarchar(kNameLength)},
             {"PrimaryPhoneNumber", MakeVarchar(kPhoneNumberLength)},
             {"VatNumber", MakeVarchar(kVatNumberLength)},
             {"PanNumber", MakeVarchar(kPanNumberLength)},
             {"Country", MakeVarchar(kCountryNameLength)},
             {"Address", MakeVarchar(kAddressLength)},
             {"Password", MakeVarchar(kPasswordLength)},
             {"VatCertificatePdf", "UUID"},
             {"PanPdf", "UUID"},
             {"BusinessFrontImage", "UUID"}});

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "RegisterAccountContactFormInfo",
            {"ContactID",
             "Name",
             "Ownername",
             "PrimaryPhoneNumber",
             "VatNumber",
             "PanNumber",
             "Country",
             "Address",
             "Password",
             "VatCertificatePdf",
             "PanPdf",
             "BusinessFrontImage"});

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("RegisterAccountContactFormInfo");

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "RegisterAccountContactFormInfo",
            {"Id",
             "ContactID",
             "Name",
             "Ownername",
             "PrimaryPhoneNumber",
             "VatNumber",
             "PanNumber",
             "Country",
             "Address",
             "Password",
             "VatCertificatePdf",
             "PanPdf",
             "BusinessFrontImage"});

        return Query.c_str();
    }

    const char *RegisterAccountContactFormInfo::GetName()
    {
        return "RegisterAccountContactFormInfo";
    }

#pragma endregion

#pragma region ItemStockLedger

    const char *ItemStockLedger::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ItemStockLedger",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"BusinessID", "UUID"},
             {"PurchaseOrderID", "UUID"},
             {"ItemId", "UUID"},
             {"StockDelta", "INTEGER"},
             {"AccountID", "UUID"},
             {"Reason", "INTEGER"},
             {"CreatedAt", "TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP"}});
        return Query.c_str();
    }

    const char *ItemStockLedger::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemStockLedger",
            {"BusinessID", "PurchaseOrderID", "ItemId", "StockDelta", "AccountID", "Reason", "CreatedAt"});
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
            {"Id", "BusinessID", "PurchaseOrderID", "ItemId", "StockDelta", "AccountID", "Reason", "CreatedAt"});
        return Query.c_str();
    }

    const char *ItemStockLedger::GetName()
    {
        return "ItemStockLedger";
    }

#pragma endregion

#pragma region PurchaseOrder

    const char *PurchaseOrder::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "PurchaseOrder",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueID", "UUID"},
             {"BusinessID", "UUID"},
             {"SupplierID", "UUID"},
             {"ItemInvoiceId", "UUID"},
             {"State", "INTEGER"},
             {"CreatedAt", "TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP"}});
        return Query.c_str();
    }

    const char *PurchaseOrder::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "PurchaseOrder",
            {"UniqueID", "BusinessID", "SupplierID", "ItemInvoiceId", "State", "CreatedAt"});
        return Query.c_str();
    }

    const char *PurchaseOrder::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("PurchaseOrder");
        return Query.c_str();
    }

    const char *PurchaseOrder::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "PurchaseOrder",
            {"Id", "UniqueID", "BusinessID", "SupplierID", "ItemInvoiceId", "State", "CreatedAt"});
        return Query.c_str();
    }

    const char *PurchaseOrder::GetName()
    {
        return "PurchaseOrder";
    }

#pragma endregion

#pragma region PurchaseOrderLine

    const char *PurchaseOrderLine::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "PurchaseOrderLine",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueID", "UUID"},
             {"PurchaseOrderID", "UUID"},
             {"ItemID", "UUID"},
             {"OrderedQuantity", "INTEGER"},
             {"ReceivedQuantity", "INTEGER"},
             {"PurchaseUnit", "INTEGER"},
             {"PurchaseToStockConversion", "DOUBLE PRECISION"},
             {"StockUnit", "INTEGER"},
             {"UnitPrice", "DOUBLE PRECISION"},
             {"DiscountPercent", "DOUBLE PRECISION"},
             {"Remarks", "TEXT"}});
        return Query.c_str();
    }

    const char *PurchaseOrderLine::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "PurchaseOrderLine",
            {"UniqueID",
             "PurchaseOrderID",
             "ItemID",
             "OrderedQuantity",
             "ReceivedQuantity",
             "PurchaseUnit",
             "PurchaseToStockConversion",
             "StockUnit",
             "UnitPrice",
             "DiscountPercent",
             "Remarks"});
        return Query.c_str();
    }

    const char *PurchaseOrderLine::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("PurchaseOrderLine");
        return Query.c_str();
    }

    const char *PurchaseOrderLine::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "PurchaseOrderLine",
            {"Id",
             "UniqueID",
             "PurchaseOrderID",
             "ItemID",
             "OrderedQuantity",
             "ReceivedQuantity",
             "PurchaseUnit",
             "PurchaseToStockConversion",
             "StockUnit",
             "UnitPrice",
             "DiscountPercent",
             "Remarks"});
        return Query.c_str();
    }

    const char *PurchaseOrderLine::GetName()
    {
        return "PurchaseOrderLine";
    }

#pragma endregion

#pragma region Account

    const char *Account::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Account",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueId", "UUID UNIQUE NOT NULL"},
             {"BusinessID", "UUID NOT NULL"},
             {"ContactFormID", "UUID NOT NULL"},
             {"ParentID", "UUID"},
             {"PasswordHash", MakeVarchar(kPasswordHashLength)},
             {"Role", "INT NOT NULL"},
             {"IsActive", "BOOLEAN NOT NULL DEFAULT TRUE"}});

        return Query.c_str();
    }

    const char *Account::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Account",
            {"UniqueId",
             "BusinessID",
             "ContactFormID",
             "ParentID",
             "PasswordHash",
             "Role",
             "IsActive"});

        return Query.c_str();
    }

    const char *Account::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("Account");

        return Query.c_str();
    }

    const char *Account::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "Account",
            {"Id",
             "UniqueId",
             "BusinessID",
             "ContactFormID",
             "ParentID",
             "PasswordHash",
             "Role",
             "IsActive"});

        return Query.c_str();
    }

    const char *Account::GetName()
    {
        return "Account";
    }

#pragma endregion

#pragma region EncryptedSecret Queries

    const char *EncryptedSecret::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "EncryptedSecret",
            {{"AccountID", "UUID PRIMARY KEY"},
             {"KeySalt", "BYTEA NOT NULL"},
             {"EncryptionNonce", "BYTEA NOT NULL"},
             {"Ciphertext", "BYTEA NOT NULL"},
             {"ActualCiphertextSize", "BIGINT NOT NULL"}});

        return Query.c_str();
    }

    const char *EncryptedSecret::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "EncryptedSecret",
            {"AccountID",
             "KeySalt",
             "EncryptionNonce",
             "Ciphertext",
             "ActualCiphertextSize"});

        return Query.c_str();
    }

    const char *EncryptedSecret::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("EncryptedSecret");
        return Query.c_str();
    }

    const char *EncryptedSecret::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "EncryptedSecret",
            {"AccountID",
             "KeySalt",
             "EncryptionNonce",
             "Ciphertext",
             "ActualCiphertextSize"});

        return Query.c_str();
    }

    const char *EncryptedSecret::GetName()
    {
        return "EncryptedSecret";
    }

#pragma endregion

#pragma region PdfDocument

    const char *PdfDocument::GetCreateQuery()
    {
        // BYTEA is fully optimized for PDF streams, compressed binaries, and documents
        static std::string Query = BuildCreateQuery(
            "PdfDocument",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueId", "UUID UNIQUE NOT NULL"},
             {"Name", MakeVarchar(kNameLength)},
             {"Data", "BYTEA NOT NULL"}});

        return Query.c_str();
    }

    const char *PdfDocument::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "PdfDocument",
            {"UniqueId",
             "Name",
             "Data"});

        return Query.c_str();
    }

    const char *PdfDocument::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("PdfDocument");
        return Query.c_str();
    }

    const char *PdfDocument::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "PdfDocument",
            {"Id",
             "UniqueId",
             "Name",
             "Data"});

        return Query.c_str();
    }

    const char *PdfDocument::GetName()
    {
        return "PdfDocument";
    }

#pragma endregion

#pragma region Image

    const char *Image::GetCreateQuery()
    {
        // Maps 'Name' to VARCHAR using your helper and 'Data' to BYTEA for binary streams
        static std::string Query = BuildCreateQuery(
            "Image",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueId", "UUID UNIQUE NOT NULL"},
             {"Name", MakeVarchar(kNameLength)},
             {"Data", "BYTEA NOT NULL"}});

        return Query.c_str();
    }

    const char *Image::GetInsertQuery()
    {
        // 'Id' is handled by SERIAL, so it is omitted from the insert targets
        static std::string Query = BuildInsertQuery(
            "Image",
            {"UniqueId",
             "Name",
             "Data"});

        return Query.c_str();
    }

    const char *Image::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("Image");
        return Query.c_str();
    }

    const char *Image::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "Image",
            {"Id",
             "UniqueId",
             "Name",
             "Data"});

        return Query.c_str();
    }

    const char *Image::GetName()
    {
        return "Image";
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
        static std::string Query = BuildCountQuery("ContactFormStateMetaInfo");

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

#pragma region ContactFormInfo

    const char *ContactFormInfo::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "ContactFormInfo",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueId", "UUID"},
             {"PhoneNumber", MakeVarchar(kPhoneNumberLength)},
             {"Email", MakeVarchar(kEmailLength)},
             {"Message", MakeVarchar(kContactMessageLength)},
             {"Type", "INTEGER"},
             {"CreatedAt", "TIMESTAMP DEFAULT CURRENT_TIMESTAMP"},
             {"State", "INTEGER"}});

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
        static std::string Query = BuildCountQuery("ContactFormInfo");

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
            {{"Id", "SERIAL PRIMARY KEY"},
             {"UniqueId", "UUID UNIQUE NOT NULL"},
             {"RegisterAccountFormId", "UUID NOT NULL"},
             {"CreatedAt", "TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP"}});

        return Query.c_str();
    }

    const char *Business::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Business",
            {"UniqueId",
             "RegisterAccountFormId",
             "CreatedAt"});

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
            {"Id",
             "UniqueId",
             "RegisterAccountFormId",
             "CreatedAt"});

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
            {{"Id", "SERIAL PRIMARY KEY"},
             {"SenderId", "UUID"},
             {"RecieverId", "UUID"},
             {"UniqueID", "UUID"},
             {"State", "INTEGER"},
             {"VatPercent", "DOUBLE PRECISION"},
             {"Total", "DOUBLE PRECISION"},
             {"SenderType", "INTEGER"},
             {"SenderServiceType", "INTEGER"},
             {"RecieverType", "INTEGER"},
             {"RecieverServiceType", "INTEGER"},
             {"CreatedAt", "TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP"},
             {"LastPaymentTicketDate", "TIMESTAMP WITH TIME ZONE"},
             {"ClosedAt", "TIMESTAMP WITH TIME ZONE"}});
        return Query.c_str();
    }

    const char *ItemInvoice::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "ItemInvoice",
            {"SenderId",
             "RecieverId",
             "UniqueID",
             "State",
             "VatPercent",
             "Total",
             "SenderType",
             "SenderServiceType",
             "RecieverType",
             "RecieverServiceType",
             "CreatedAt",
             "LastPaymentTicketDate",
             "ClosedAt"});
        return Query.c_str();
    }

    const char *ItemInvoice::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "ItemInvoice",
            {"Id",
             "SenderId",
             "RecieverId",
             "UniqueID",
             "State",
             "VatPercent",
             "Total",
             "SenderType",
             "SenderServiceType",
             "RecieverType",
             "RecieverServiceType",
             "CreatedAt",
             "LastPaymentTicketDate",
             "ClosedAt"});
        return Query.c_str();
    }

    const char *ItemInvoice::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("ItemInvoice");
        return Query.c_str();
    }

    const char *ItemInvoice::GetName()
    {
        return "ItemInvoice";
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

#pragma region Item
    const char *Item::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Item",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"BusinessID", "UUID"},
             {"CategoryId", "UUID"},
             {"UniqueID", "UUID"},
             {"Name", "TEXT"},
             {"LowStockThresold", "INTEGER"},
             {"TracksStock", "BOOLEAN"},
             {"Description", "TEXT"},
             {"StockUnit", "INTEGER"},
             {"DefaultPurchaseConversion", "DOUBLE PRECISION"},
             {"DefaultPurchaseUnit", "INTEGER"}});
        return Query.c_str();
    }

    const char *Item::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Item",
            {"BusinessID",
             "CategoryId",
             "UniqueID",
             "Name",
             "LowStockThresold",
             "TracksStock",
             "Description",
             "StockUnit",
             "DefaultPurchaseConversion",
             "DefaultPurchaseUnit"});
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
            {"Id",
             "BusinessID",
             "CategoryId",
             "UniqueID",
             "Name",
             "LowStockThresold",
             "TracksStock",
             "Description",
             "StockUnit",
             "DefaultPurchaseConversion",
             "DefaultPurchaseUnit"});
        return Query.c_str();
    }

    const char *Item::GetName()
    {
        return "Item";
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
                {"Name", "TEXT"},
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

#pragma region Suppliers

    const char *Suppliers::GetCreateQuery()
    {
        static std::string Query = BuildCreateQuery(
            "Suppliers",
            {{"Id", "SERIAL PRIMARY KEY"},
             {"BusinessID", "UUID"},
             {"UnqiueId", "UUID"},
             {"Name", "TEXT"},
             {"PhoneNumber", "TEXT"},
             {"PanNumber", "TEXT"},
             {"OpeningBalance", "DOUBLE PRECISION"}});
        return Query.c_str();
    }

    const char *Suppliers::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "Suppliers",
            {"BusinessID",
             "UnqiueId",
             "Name",
             "PhoneNumber",
             "PanNumber",
             "OpeningBalance"});
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
            {"Id",
             "BusinessID",
             "UnqiueId",
             "Name",
             "PhoneNumber",
             "PanNumber",
             "OpeningBalance"});
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
                {"UniqueID", "UUID"},
                {"BusinessID", "UUID"},
                {"Name", "TEXT"},
                {"BasePrice", "DOUBLE PRECISION"},
                {"Description", "TEXT"},
                {"State", "INTEGER"},
                {"BedCount", "INTEGER"}
            });
        return Query.c_str();
    }

    const char *RoomInfo::GetInsertQuery()
    {
        static std::string Query = BuildInsertQuery(
            "RoomInfo",
            {
                "UniqueID",
                "BusinessID",
                "Name",
                "BasePrice",
                "Description",
                "State",
                "BedCount"
            });
        return Query.c_str();
    }

    const char *RoomInfo::GetByIdQuery()
    {
        static std::string Query = BuildByIdQuery(
            "RoomInfo",
            {
                "Id",
                "UniqueID",
                "BusinessID",
                "Name",
                "BasePrice",
                "Description",
                "State",
                "BedCount"
            });
        return Query.c_str();
    }

    const char *RoomInfo::GetCountQuery()
    {
        static std::string Query = BuildCountQuery("RoomInfo");
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
