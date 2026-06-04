#include <iostream>
#include <sstream>
#include <string>
#include "Tables.h"

namespace NepBill
{
    const char *BusinessInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE BusinessInfo ("
               << "Id INTEGER PRIMARY KEY, "
               << "Name VARCHAR(" << kNameLength << "), "
               << "VatNumber VARCHAR(" << BusinessInfoVatNumberLength << "), "
               << "Address VARCHAR(" << BusinessInfoAddressLength << "), "
               << "PanNumber VARCHAR(" << kPanNumberLength << "), "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "FiscalYearStart INTEGER, "
               << "UniqueID BLOB"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BusinessInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE BusinessInfo ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "Name VARCHAR(" << kNameLength << "), "
               << "VatNumber VARCHAR(" << BusinessInfoVatNumberLength << "), "
               << "Address VARCHAR(" << BusinessInfoAddressLength << "), "
               << "PanNumber VARCHAR(" << kPanNumberLength << "), "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "FiscalYearStart BIGINT, "
               << "UniqueID UUID"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BusinessInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO BusinessInfo "
               "(Id, Name, VatNumber, Address, PanNumber, PhoneNumber, FiscalYearStart, UniqueID) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *BusinessInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, Name, VatNumber, Address, PanNumber, PhoneNumber, FiscalYearStart, UniqueID "
               "FROM BusinessInfo "
               "WHERE Id = ?";
    }

    const char *BusinessInfo::GetName()
    {
        return "BusinessInfo";
    }

    const char *BusinessInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BusinessInfo";
    }

    const char *Customer::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Customer ("
               << "Id INTEGER PRIMARY KEY, "
               << "UniqueID BLOB, "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "Name VARCHAR(" << kNameLength << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Customer::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Customer ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "UniqueID UUID, "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "Name VARCHAR(" << kNameLength << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Customer::GetInsertQuerySqlite()
    {
        return "INSERT INTO Customer "
               "(Id, UniqueID, PhoneNumber, Name) "
               "VALUES (?, ?, ?, ?)";
    }

    const char *Customer::GetByIdQuery()
    {
        return "SELECT * FROM Customer WHERE Id = ?";
    }

    const char *Customer::GetName()
    {
        return "Customer";
    }

    const char *Customer::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Customer";
    }

    const char *Invoice::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Invoice ("
               << "Id INTEGER PRIMARY KEY, "
               << "SenderId BLOB, "
               << "RecieverId BLOB, "
               << "UniqueID BLOB, "
               << "State INTEGER, "
               << "VatPercent REAL, "
               << "LineTotal REAL, "
               << "SenderType INTEGER, "
               << "RecieverType INTEGER, "
               << "CreatedAt INTEGER, "
               << "LastPaymentTicketDate INTEGER, "
               << "ClosedAt INTEGER"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Invoice::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Invoice ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "SenderId UUID, "
               << "RecieverId UUID, "
               << "UniqueID UUID, "
               << "State INTEGER, "
               << "VatPercent REAL, "
               << "LineTotal REAL, "
               << "SenderType INTEGER, "
               << "RecieverType INTEGER, "
               << "CreatedAt BIGINT, "
               << "LastPaymentTicketDate BIGINT, "
               << "ClosedAt BIGINT"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Invoice::GetInsertQuerySqlite()
    {
        return "INSERT INTO Invoice "
               "(Id, SenderId, RecieverId, UniqueID, State, VatPercent, LineTotal, SenderType, RecieverType, CreatedAt, LastPaymentTicketDate, ClosedAt) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *Invoice::GetByIdQuery()
    {
        return "SELECT * FROM Invoice WHERE Id = ?";
    }

    const char *Invoice::GetName()
    {
        return "Invoice";
    }

    const char *Invoice::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Invoice";
    }

    const char *InvoiceLine::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE InvoiceLine ("
               << "Id INTEGER PRIMARY KEY, "
               << "InvoiceId BLOB, "
               << "ItemId BLOB, "
               << "UnqiueId BLOB, "
               << "OrderedQuantity INTEGER, "
               << "DeliveredQuantity INTEGER, "
               << "ItemWeight REAL, "
               << "UnitPrice REAL, "
               << "UnitDiscountPercet REAL, "
               << "LineTotal REAL"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *InvoiceLine::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE InvoiceLine ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "InvoiceId UUID, "
               << "ItemId UUID, "
               << "UnqiueId UUID, "
               << "OrderedQuantity INTEGER, "
               << "DeliveredQuantity INTEGER, "
               << "ItemWeight REAL, "
               << "UnitPrice REAL, "
               << "UnitDiscountPercet REAL, "
               << "LineTotal REAL"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *InvoiceLine::GetInsertQuerySqlite()
    {
        return "INSERT INTO InvoiceLine "
               "(Id, InvoiceId, ItemId, UnqiueId, OrderedQuantity, DeliveredQuantity, ItemWeight, UnitPrice, UnitDiscountPercet, LineTotal) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *InvoiceLine::GetByIdQuery()
    {
        return "SELECT * FROM InvoiceLine WHERE Id = ?";
    }

    const char *InvoiceLine::GetName()
    {
        return "InvoiceLine";
    }

    const char *InvoiceLine::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM InvoiceLine";
    }

    const char *PaymentTicket::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE PaymentTicket ("
               << "Id INTEGER PRIMARY KEY, "
               << "PaidAmount REAL, "
               << "Sender INTEGER, "
               << "Receiver INTEGER, "
               << "InvoiceID BLOB, "
               << "UniqueID BLOB, "
               << "PaidType INTEGER, "
               << "CreatedAt INTEGER, "
               << "Note VARCHAR(" << kPaymentTicketNotes << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *PaymentTicket::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE PaymentTicket ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "PaidAmount REAL, "
               << "Sender INTEGER, "
               << "Receiver INTEGER, "
               << "InvoiceID UUID, "
               << "UniqueID UUID, "
               << "PaidType INTEGER, "
               << "CreatedAt BIGINT, "
               << "Note VARCHAR(" << kPaymentTicketNotes << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *PaymentTicket::GetInsertQuerySqlite()
    {
        return "INSERT INTO PaymentTicket "
               "(Id, PaidAmount, Sender, Receiver, InvoiceID, UniqueID, PaidType, CreatedAt, Note) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *PaymentTicket::GetByIdQuery()
    {
        return "SELECT * FROM PaymentTicket WHERE Id = ?";
    }

    const char *PaymentTicket::GetName()
    {
        return "PaymentTicket";
    }

    const char *PaymentTicket::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM PaymentTicket";
    }

    const char *ItemCategory::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE ItemCategory ("
               << "Id INTEGER PRIMARY KEY, "
               << "UniqueID BLOB, "
               << "Name VARCHAR(" << kNameLength << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemCategory::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE ItemCategory ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "UniqueID UUID, "
               << "Name VARCHAR(" << kNameLength << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemCategory::GetInsertQuerySqlite()
    {
        return "INSERT INTO ItemCategory "
               "(Id, UniqueID, Name) "
               "VALUES (?, ?, ?)";
    }

    const char *ItemCategory::GetByIdQuery()
    {
        return "SELECT * FROM ItemCategory WHERE Id = ?";
    }

    const char *ItemCategory::GetName()
    {
        return "ItemCategory";
    }

    const char *ItemCategory::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM ItemCategory";
    }

    const char *Item::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Item ("
               << "Id INTEGER PRIMARY KEY, "
               << "CategoryId BLOB, "
               << "UniqueID BLOB, "
               << "Name VARCHAR(" << ItemNameLength << "), "
               << "LowStockThreshold INTEGER, "
               << "CostPrice REAL, "
               << "SalesPrice REAL, "
               << "DiscountPercent REAL, "
               << "Description VARCHAR(" << kDescription << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Item::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Item ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "CategoryId UUID, "
               << "UniqueID UUID, "
               << "Name VARCHAR(" << ItemNameLength << "), "
               << "LowStockThreshold INTEGER, "
               << "CostPrice REAL, "
               << "SalesPrice REAL, "
               << "DiscountPercent REAL, "
               << "Description VARCHAR(" << kDescription << ")"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Item::GetInsertQuerySqlite()
    {
        return "INSERT INTO Item "
               "(Id, CategoryId, UniqueID, Name, LowStockThreshold, CostPrice, SalesPrice, DiscountPercent, Description) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *Item::GetByIdQuery()
    {
        return "SELECT * FROM Item WHERE Id = ?";
    }

    const char *Item::GetName()
    {
        return "Item";
    }

    const char *Item::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Item";
    }

    const char *ItemStockLedger::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE ItemStockLedger ("
               << "Id INTEGER PRIMARY KEY, "
               << "ItemId BLOB, "
               << "StockDelta INTEGER, "
               << "CreatedAt INTEGER"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemStockLedger::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE ItemStockLedger ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "ItemId UUID, "
               << "StockDelta INTEGER, "
               << "CreatedAt BIGINT"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemStockLedger::GetInsertQuerySqlite()
    {
        return "INSERT INTO ItemStockLedger "
               "(Id, ItemId, StockDelta, CreatedAt) "
               "VALUES (?, ?, ?, ?)";
    }

    const char *ItemStockLedger::GetByIdQuery()
    {
        return "SELECT * FROM ItemStockLedger WHERE Id = ?";
    }

    const char *ItemStockLedger::GetName()
    {
        return "ItemStockLedger";
    }

    const char *ItemStockLedger::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM ItemStockLedger";
    }

    const char *Suppliers::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Suppliers ("
               << "Id INTEGER PRIMARY KEY, "
               << "UniqueID BLOB, "
               << "Name VARCHAR(" << kNameLength << "), "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "PanNumber VARCHAR(" << kPanNumberLength << "), "
               << "OpeningBalance REAL"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Suppliers::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE Suppliers ("
               << "Id BIGSERIAL PRIMARY KEY, "
               << "UniqueID UUID, "
               << "Name VARCHAR(" << kNameLength << "), "
               << "PhoneNumber VARCHAR(" << kPhoneNumberLength << "), "
               << "PanNumber VARCHAR(" << kPanNumberLength << "), "
               << "OpeningBalance REAL"
               << ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Suppliers::GetInsertQuerySqlite()
    {
        return "INSERT INTO Suppliers "
               "(Id, UniqueID, Name, PhoneNumber, PanNumber, OpeningBalance) "
               "VALUES (?, ?, ?, ?, ?, ?)";
    }

    const char *Suppliers::GetByIdQuery()
    {
        return "SELECT * FROM Suppliers WHERE Id = ?";
    }

    const char *Suppliers::GetName()
    {
        return "Suppliers";
    }

    const char *Suppliers::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Suppliers";
    }
} // namespace NepBill
