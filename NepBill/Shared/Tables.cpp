#include <iostream>
#include <string>
#include <strstream>
#include <iosfwd>
#include <sstream>

#include "Tables.h"

namespace NepBill
{
    // ============================
    // HotelInfo
    // ============================

    const char *HotelInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS HotelInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "VatNumber VARCHAR("
               << kVatNumberLength << "), "
                                      "Address VARCHAR("
               << kAddressLength << "), "
                                    "PanNumber VARCHAR("
               << kPanNumberLength << "), "
                                      "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "Country VARCHAR("
               << kCountryNameLength << "), "
                                        "UniqueID BLOB"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *HotelInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS HotelInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "VatNumber VARCHAR("
               << kVatNumberLength << "), "
                                      "Address VARCHAR("
               << kAddressLength << "), "
                                    "PanNumber VARCHAR("
               << kPanNumberLength << "), "
                                      "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "Country VARCHAR("
               << kCountryNameLength << "), "
                                        "UniqueID UUID"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *HotelInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO HotelInfo "
               "(Name, VatNumber, Address, PanNumber, PhoneNumber, Country, UniqueID) "
               "VALUES (?, ?, ?, ?, ?, ?, ?)";
    }

    const char *HotelInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM HotelInfo";
    }

    const char *HotelInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, Name, VatNumber, Address, PanNumber, PhoneNumber, Country, UniqueID "
               "FROM HotelInfo WHERE Id = ?";
    }

    const char *HotelInfo::GetName()
    {
        return "HotelInfo";
    }

    // ============================
    // Invoice
    // ============================

    const char *Invoice::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Invoice ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "SenderId BLOB, "
                  "RecieverId BLOB, "
                  "UniqueID BLOB, "
                  "State INTEGER NOT NULL, "
                  "VatPercent REAL DEFAULT 0, "
                  "LineTotal REAL DEFAULT 0, "
                  "SenderType INTEGER, "
                  "RecieverType INTEGER, "
                  "CreatedAt INTEGER, "
                  "LastPaymentTicketDate INTEGER, "
                  "ClosedAt INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Invoice::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Invoice ("
                  "Id SERIAL PRIMARY KEY, "
                  "SenderId UUID, "
                  "RecieverId UUID, "
                  "UniqueID UUID, "
                  "State INTEGER NOT NULL, "
                  "VatPercent DOUBLE PRECISION DEFAULT 0, "
                  "LineTotal DOUBLE PRECISION DEFAULT 0, "
                  "SenderType INTEGER, "
                  "RecieverType INTEGER, "
                  "CreatedAt BIGINT, "
                  "LastPaymentTicketDate BIGINT, "
                  "ClosedAt BIGINT"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Invoice::GetInsertQuerySqlite()
    {
        return "INSERT INTO Invoice "
               "(SenderId, RecieverId, UniqueID, State, VatPercent, "
               "LineTotal, SenderType, RecieverType, CreatedAt, "
               "LastPaymentTicketDate, ClosedAt) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *Invoice::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Invoice";
    }

    const char *Invoice::GetByIdQuery()
    {
        return "SELECT "
               "Id, SenderId, RecieverId, UniqueID, State, VatPercent, "
               "LineTotal, SenderType, RecieverType, CreatedAt, "
               "LastPaymentTicketDate, ClosedAt "
               "FROM Invoice WHERE Id = ?";
    }

    const char *Invoice::GetName()
    {
        return "Invoice";
    }

#pragma region InvoiceLine

    const char *InvoiceLine::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS InvoiceLine ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "InvoiceId BLOB, "
                  "ItemId BLOB, "
                  "UnqiueId BLOB, "
                  "OrderedQuantity INTEGER, "
                  "DeliveredQuantity INTEGER, "
                  "ItemWeight REAL, "
                  "UnitPrice REAL, "
                  "UnitDiscountPercet REAL, "
                  "LineTotal REAL"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *InvoiceLine::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS InvoiceLine ("
                  "Id SERIAL PRIMARY KEY, "
                  "InvoiceId UUID, "
                  "ItemId UUID, "
                  "UnqiueId UUID, "
                  "OrderedQuantity INTEGER, "
                  "DeliveredQuantity INTEGER, "
                  "ItemWeight DOUBLE PRECISION, "
                  "UnitPrice DOUBLE PRECISION, "
                  "UnitDiscountPercet DOUBLE PRECISION, "
                  "LineTotal DOUBLE PRECISION"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *InvoiceLine::GetInsertQuerySqlite()
    {
        return "INSERT INTO InvoiceLine "
               "(InvoiceId, ItemId, UnqiueId, OrderedQuantity, DeliveredQuantity, "
               "ItemWeight, UnitPrice, UnitDiscountPercet, LineTotal) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *InvoiceLine::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM InvoiceLine";
    }

    const char *InvoiceLine::GetByIdQuery()
    {
        return "SELECT "
               "Id, InvoiceId, ItemId, UnqiueId, OrderedQuantity, DeliveredQuantity, "
               "ItemWeight, UnitPrice, UnitDiscountPercet, LineTotal "
               "FROM InvoiceLine WHERE Id = ?";
    }

    const char *InvoiceLine::GetName()
    {
        return "InvoiceLine";
    }
#pragma endregion

#pragma region PaymnetTicket
    const char *PaymentTicket::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS PaymentTicket ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "PaidAmount REAL, "
                  "Sender INTEGER, "
                  "Receiver INTEGER, "
                  "TargetType INTEGER, "
                  "InvoiceID BLOB, "
                  "UniqueID BLOB, "
                  "PaidType INTEGER, "
                  "CreatedAt INTEGER, "
                  "Note VARCHAR("
               << kPaymentTicketNotesLength << ")"
                                               ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *PaymentTicket::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS PaymentTicket ("
                  "Id SERIAL PRIMARY KEY, "
                  "PaidAmount DOUBLE PRECISION, "
                  "Sender INTEGER, "
                  "Receiver INTEGER, "
                  "TargetType INTEGER, "
                  "InvoiceID UUID, "
                  "UniqueID UUID, "
                  "PaidType INTEGER, "
                  "CreatedAt BIGINT, "
                  "Note VARCHAR("
               << kPaymentTicketNotesLength << ")"
                                               ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *PaymentTicket::GetInsertQuerySqlite()
    {
        return "INSERT INTO PaymentTicket "
               "(PaidAmount, Sender, Receiver, TargetType, "
               "InvoiceID, UniqueID, PaidType, CreatedAt, Note) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *PaymentTicket::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM PaymentTicket";
    }

    const char *PaymentTicket::GetByIdQuery()
    {
        return "SELECT "
               "Id, PaidAmount, Sender, Receiver, InvoiceID, UniqueID, "
               "PaidType, CreatedAt, Note "
               "FROM PaymentTicket WHERE Id = ?";
    }

    const char *PaymentTicket::GetName()
    {
        return "PaymentTicket";
    }
#pragma endregion

#pragma region ItemCategory

    const char *ItemCategory::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS ItemCategory ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UniqueID BLOB, "
                  "Name VARCHAR("
               << kNameLength << ")"
                                 ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemCategory::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS ItemCategory ("
                  "Id SERIAL PRIMARY KEY, "
                  "UniqueID UUID, "
                  "Name VARCHAR("
               << kNameLength << ")"
                                 ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemCategory::GetInsertQuerySqlite()
    {
        return "INSERT INTO ItemCategory "
               "(UniqueID, Name) "
               "VALUES (?, ?)";
    }

    const char *ItemCategory::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM ItemCategory";
    }

    const char *ItemCategory::GetByIdQuery()
    {
        return "SELECT Id, UniqueID, Name "
               "FROM ItemCategory WHERE Id = ?";
    }

    const char *ItemCategory::GetName()
    {
        return "ItemCategory";
    }

#pragma endregion

#pragma region Item

    const char *Item::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Item ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "CategoryId BLOB, "
                  "UniqueID BLOB, "
                  "Name VARCHAR("
               << ItemNameLength << "), "
                                    "LowStockThresold INTEGER, "
                                    "CostPrice REAL, "
                                    "SalesPrice REAL, "
                                    "DiscountPercent REAL, "
                                    "Description VARCHAR("
               << ItemDescriptionLength << ")"
                                           ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Item::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Item ("
                  "Id SERIAL PRIMARY KEY, "
                  "CategoryId UUID, "
                  "UniqueID UUID, "
                  "Name VARCHAR("
               << ItemNameLength << "), "
                                    "LowStockThresold INTEGER, "
                                    "CostPrice DOUBLE PRECISION, "
                                    "SalesPrice DOUBLE PRECISION, "
                                    "DiscountPercent DOUBLE PRECISION, "
                                    "Description VARCHAR("
               << ItemDescriptionLength << ")"
                                           ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Item::GetInsertQuerySqlite()
    {
        return "INSERT INTO Item "
               "(CategoryId, UniqueID, Name, LowStockThresold, CostPrice, "
               "SalesPrice, DiscountPercent, Description) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *Item::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Item";
    }

    const char *Item::GetByIdQuery()
    {
        return "SELECT "
               "Id, CategoryId, UniqueID, Name, LowStockThresold, CostPrice, "
               "SalesPrice, DiscountPercent, Description "
               "FROM Item WHERE Id = ?";
    }

    const char *Item::GetName()
    {
        return "Item";
    }

#pragma endregion

#pragma region ItemStockLedger

    const char *ItemStockLedger::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS ItemStockLedger ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "ItemId BLOB, "
                  "StockDelta INTEGER, "
                  "CreatedAt INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemStockLedger::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS ItemStockLedger ("
                  "Id SERIAL PRIMARY KEY, "
                  "ItemId UUID, "
                  "StockDelta INTEGER, "
                  "CreatedAt BIGINT"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *ItemStockLedger::GetInsertQuerySqlite()
    {
        return "INSERT INTO ItemStockLedger "
               "(ItemId, StockDelta, CreatedAt) "
               "VALUES (?, ?, ?)";
    }

    const char *ItemStockLedger::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM ItemStockLedger";
    }

    const char *ItemStockLedger::GetByIdQuery()
    {
        return "SELECT Id, ItemId, StockDelta, CreatedAt "
               "FROM ItemStockLedger WHERE Id = ?";
    }

    const char *ItemStockLedger::GetName()
    {
        return "ItemStockLedger";
    }

#pragma endregion

#pragma region Suppliers

    const char *Suppliers::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Suppliers ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UnqiueId BLOB, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "PanNumber VARCHAR("
               << kPanNumberLength << "), "
                                      "OpeningBalance REAL"
                                      ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Suppliers::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS Suppliers ("
                  "Id SERIAL PRIMARY KEY, "
                  "UnqiueId UUID, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "PanNumber VARCHAR("
               << kPanNumberLength << "), "
                                      "OpeningBalance DOUBLE PRECISION"
                                      ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *Suppliers::GetInsertQuerySqlite()
    {
        return "INSERT INTO Suppliers "
               "(UnqiueId, Name, PhoneNumber, PanNumber, OpeningBalance) "
               "VALUES (?, ?, ?, ?, ?)";
    }

    const char *Suppliers::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM Suppliers";
    }

    const char *Suppliers::GetByIdQuery()
    {
        return "SELECT Id, UnqiueId, Name, PhoneNumber, PanNumber, OpeningBalance "
               "FROM Suppliers WHERE Id = ?";
    }

    const char *Suppliers::GetName()
    {
        return "Suppliers";
    }

#pragma endregion

#pragma region SuppliersItemsTable

    const char *SuppliersItemTable::GetCreateQuerySqlite()
    {
        return "CREATE TABLE IF NOT EXISTS SuppliersItemTable ("
               "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "SupplierID BLOB, "
               "ItemID BLOB"
               ")";
    }

    const char *SuppliersItemTable::GetCreateQueryPostgreSQL()
    {
        return "CREATE TABLE IF NOT EXISTS SuppliersItemTable ("
               "Id SERIAL PRIMARY KEY, "
               "SupplierID UUID, "
               "ItemID UUID"
               ")";
    }

    const char *SuppliersItemTable::GetInsertQuerySqlite()
    {
        return "INSERT INTO SuppliersItemTable "
               "(SupplierID, ItemID) "
               "VALUES (?, ?)";
    }

    const char *SuppliersItemTable::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM SuppliersItemTable";
    }

    const char *SuppliersItemTable::GetByIdQuery()
    {
        return "SELECT Id, SupplierID, ItemID "
               "FROM SuppliersItemTable WHERE Id = ?";
    }

    const char *SuppliersItemTable::GetName()
    {
        return "SuppliersItemTable";
    }

#pragma endregion

#pragma region RoomInfo

    const char *RoomInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS RoomInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "BasePrice REAL, "
                                 "BedCount INTEGER, "
                                 "Description VARCHAR("
               << RoomDescriptionLength << "), "
                                           "UniqueID BLOB, "
                                           "State INTEGER"
                                           ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *RoomInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS RoomInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "BasePrice DOUBLE PRECISION, "
                                 "BedCount INTEGER, "
                                 "Description VARCHAR("
               << RoomDescriptionLength << "), "
                                           "UniqueID UUID, "
                                           "State INTEGER"
                                           ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *RoomInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO RoomInfo "
               "(Name, BasePrice, BedCount, Description, UniqueID, State) "
               "VALUES (?, ?, ?, ?, ?, ?)";
    }

    const char *RoomInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM RoomInfo";
    }

    const char *RoomInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, Name, BasePrice, BedCount, Description, UniqueID, State "
               "FROM RoomInfo WHERE Id = ?";
    }

    const char *RoomInfo::GetName()
    {
        return "RoomInfo";
    }

#pragma endregion

#pragma region RoomFacilityInfo

    const char *RoomFacilityInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS RoomFacilityInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UniqueID BLOB, "
                  "RoomID BLOB, "
                  "FacilityInfo VARCHAR("
               << RoomFacilityLength << "), "
                                        "FacilityPrice REAL"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS RoomFacilityInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "UniqueID UUID, "
                  "RoomID UUID, "
                  "FacilityInfo VARCHAR("
               << RoomFacilityLength << "), "
                                        "FacilityPrice DOUBLE PRECISION"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *RoomFacilityInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO RoomFacilityInfo "
               "(UniqueID, RoomID, FacilityInfo, FacilityPrice) "
               "VALUES (?, ?, ?, ?)";
    }

    const char *RoomFacilityInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM RoomFacilityInfo";
    }

    const char *RoomFacilityInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, UniqueID, RoomID, FacilityInfo, FacilityPrice "
               "FROM RoomFacilityInfo WHERE Id = ?";
    }

    const char *RoomFacilityInfo::GetName()
    {
        return "RoomFacilityInfo";
    }

#pragma endregion

#pragma region GuestInfo

    const char *GuestInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS GuestInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UniqueID BLOB, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "Country VARCHAR("
               << kCountryNameLength << ")"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *GuestInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS GuestInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "UniqueID UUID, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "Country VARCHAR("
               << kCountryNameLength << ")"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *GuestInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO GuestInfo "
               "(UniqueID, Name, PhoneNumber, Country) "
               "VALUES (?, ?, ?, ?)";
    }

    const char *GuestInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM GuestInfo";
    }

    const char *GuestInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, UniqueID, Name, PhoneNumber, Country "
               "FROM GuestInfo WHERE Id = ?";
    }

    const char *GuestInfo::GetName()
    {
        return "GuestInfo";
    }

#pragma endregion

#pragma region BookingInfo

    const char *BookingInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UniqueID BLOB, "
                  "InvoiceID BLOB, "
                  "RoomID BLOB, "
                  "CreatedAt INTEGER, "
                  "CheckIn INTEGER, "
                  "CheckOut INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "UniqueID UUID, "
                  "InvoiceID UUID, "
                  "RoomID UUID, "
                  "CreatedAt BIGINT, "
                  "CheckIn BIGINT, "
                  "CheckOut BIGINT"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO BookingInfo "
               "(UniqueID, InvoiceID, RoomID, CreatedAt, CheckIn, CheckOut) "
               "VALUES (?, ?, ?, ?, ?, ?)";
    }

    const char *BookingInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BookingInfo";
    }

    const char *BookingInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, UniqueID, GuestID, InvoiceID, RoomID, CreatedAt, CheckIn, CheckOut "
               "FROM BookingInfo WHERE Id = ?";
    }

    const char *BookingInfo::GetName()
    {
        return "BookingInfo";
    }

#pragma endregion

#pragma region BookingAdditionalCostInfo

    const char *BookingAdditionalCostInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingAdditionalCostInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UnqiueID BLOB, "
                  "BookingID BLOB, "
                  "RoomFacility BLOB, "
                  "PricePerUnit REAL, "
                  "UnitCount INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingAdditionalCostInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingAdditionalCostInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "UnqiueID UUID, "
                  "BookingID UUID, "
                  "RoomFacility UUID, "
                  "PricePerUnit DOUBLE PRECISION, "
                  "UnitCount INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingAdditionalCostInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO BookingAdditionalCostInfo "
               "(UnqiueID, BookingID, RoomFacility, PricePerUnit, UnitCount) "
               "VALUES (?, ?, ?, ?, ?)";
    }

    const char *BookingAdditionalCostInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BookingAdditionalCostInfo";
    }

    const char *BookingAdditionalCostInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, UnqiueID, BookingID, RoomFacility, PricePerUnit, UnitCount "
               "FROM BookingAdditionalCostInfo WHERE Id = ?";
    }

    const char *BookingAdditionalCostInfo::GetName()
    {
        return "BookingAdditionalCostInfo";
    }

#pragma endregion

#pragma region StaffInfo

    const char *StaffInfo::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffInfo ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UnqiueID BLOB, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "EmergencyPhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "IsActive INTEGER, "
                                        "SalaryPerDay REAL"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffInfo::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffInfo ("
                  "Id SERIAL PRIMARY KEY, "
                  "UnqiueID UUID, "
                  "Name VARCHAR("
               << kNameLength << "), "
                                 "PhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "EmergencyPhoneNumber VARCHAR("
               << kPhoneNumberLength << "), "
                                        "IsActive INTEGER, "
                                        "SalaryPerDay DOUBLE PRECISION"
                                        ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffInfo::GetInsertQuerySqlite()
    {
        return "INSERT INTO StaffInfo "
               "(UnqiueID, Name, PhoneNumber, EmergencyPhoneNumber, IsActive, SalaryPerDay) "
               "VALUES (?, ?, ?, ?, ?, ?)";
    }

    const char *StaffInfo::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM StaffInfo";
    }

    const char *StaffInfo::GetByIdQuery()
    {
        return "SELECT "
               "Id, UnqiueID, Name, PhoneNumber, EmergencyPhoneNumber, IsActive, SalaryPerDay "
               "FROM StaffInfo WHERE Id = ?";
    }

    const char *StaffInfo::GetName()
    {
        return "StaffInfo";
    }

#pragma endregion

#pragma region StaffLedger

    const char *StaffLedger::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffLedger ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "StaffID BLOB, "
                  "CheckIn INTEGER, "
                  "CheckOut INTEGER, "
                  "PaymentRelated INTEGER, "
                  "SalaryPaymentID BLOB"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffLedger::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffLedger ("
                  "Id SERIAL PRIMARY KEY, "
                  "StaffID UUID, "
                  "CheckIn BIGINT, "
                  "CheckOut BIGINT, "
                  "PaymentRelated INTEGER, "
                  "SalaryPaymentID UUID"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffLedger::GetInsertQuerySqlite()
    {
        return "INSERT INTO StaffLedger "
               "(StaffID, CheckIn, CheckOut, PaymentRelated, SalaryPaymentID) "
               "VALUES (?, ?, ?, ?, ?)";
    }

    const char *StaffLedger::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM StaffLedger";
    }

    const char *StaffLedger::GetByIdQuery()
    {
        return "SELECT "
               "Id, StaffID, CheckIn, CheckOut, PaymentRelated, SalaryPaymentID "
               "FROM StaffLedger WHERE Id = ?";
    }

    const char *StaffLedger::GetName()
    {
        return "StaffLedger";
    }

#pragma endregion

#pragma region StaffSalaryPayment

    const char *StaffSalaryPayment::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffSalaryPayment ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "UnqiueID BLOB, "
                  "StaffID BLOB, "
                  "Amount REAL, "
                  "Date INTEGER, "
                  "Description VARCHAR("
               << SalaryPaymentDescriptionLength << ")"
                                                    ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS StaffSalaryPayment ("
                  "Id SERIAL PRIMARY KEY, "
                  "UnqiueID UUID, "
                  "StaffID UUID, "
                  "Amount DOUBLE PRECISION, "
                  "Date BIGINT, "
                  "Description VARCHAR("
               << SalaryPaymentDescriptionLength << ")"
                                                    ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *StaffSalaryPayment::GetInsertQuerySqlite()
    {
        return "INSERT INTO StaffSalaryPayment "
               "(UnqiueID, StaffID, Amount, Date, Description) "
               "VALUES (?, ?, ?, ?, ?)";
    }

    const char *StaffSalaryPayment::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM StaffSalaryPayment";
    }

    const char *StaffSalaryPayment::GetByIdQuery()
    {
        return "SELECT "
               "Id, UnqiueID, StaffID, Amount, Date, Description "
               "FROM StaffSalaryPayment WHERE Id = ?";
    }

    const char *StaffSalaryPayment::GetName()
    {
        return "StaffSalaryPayment";
    }

#pragma endregion

#pragma region BookingGuestTable

    const char *BookingGuestTable::GetCreateQuerySqlite()
    {
        return "CREATE TABLE IF NOT EXISTS BookingGuestTable ("
               "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "GuestID BLOB, "
               "BookingID BLOB"
               ")";
    }

    const char *BookingGuestTable::GetCreateQueryPostgreSQL()
    {
        return "CREATE TABLE IF NOT EXISTS BookingGuestTable ("
               "Id SERIAL PRIMARY KEY, "
               "GuestID UUID, "
               "BookingID UUID"
               ")";
    }

    const char *BookingGuestTable::GetInsertQuerySqlite()
    {
        return "INSERT INTO BookingGuestTable "
               "(GuestID, BookingID) "
               "VALUES (?, ?)";
    }

    const char *BookingGuestTable::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BookingGuestTable";
    }

    const char *BookingGuestTable::GetByIdQuery()
    {
        return "SELECT "
               "Id, "
               "GuestID, "
               "BookingID "
               "FROM BookingGuestTable "
               "WHERE Id = ?";
    }

    const char *BookingGuestTable::GetName()
    {
        return "BookingGuestTable";
    }
#pragma endregion

#pragma region BookingInvoice
    const char *BookingInvoice::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInvoice ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "GuestId BLOB, "
                  "UniqueID BLOB, "
                  "State INTEGER NOT NULL, "
                  "VatPercent REAL DEFAULT 0, "
                  "LineTotal REAL DEFAULT 0, "
                  "CreatedAt INTEGER, "
                  "LastPaymentTicketDate INTEGER, "
                  "ClosedAt INTEGER"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInvoice::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInvoice ("
                  "Id SERIAL PRIMARY KEY, "
                  "GuestId UUID, "
                  "UniqueID UUID, "
                  "State INTEGER NOT NULL, "
                  "VatPercent DOUBLE PRECISION DEFAULT 0, "
                  "LineTotal DOUBLE PRECISION DEFAULT 0, "
                  "CreatedAt BIGINT, "
                  "LastPaymentTicketDate BIGINT, "
                  "ClosedAt BIGINT"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInvoice::GetInsertQuerySqlite()
    {
        return "INSERT INTO BookingInvoice "
               "(GuestId, UniqueID, State, VatPercent, LineTotal, "
               "CreatedAt, LastPaymentTicketDate, ClosedAt) "
               "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";
    }

    const char *BookingInvoice::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BookingInvoice";
    }

    const char *BookingInvoice::GetByIdQuery()
    {
        return "SELECT "
               "Id, "
               "GuestId, "
               "UniqueID, "
               "State, "
               "VatPercent, "
               "LineTotal, "
               "CreatedAt, "
               "LastPaymentTicketDate, "
               "ClosedAt "
               "FROM BookingInvoice "
               "WHERE Id = ?";
    }

    const char *BookingInvoice::GetName()
    {
        return "BookingInvoice";
    }
#pragma endregion

#pragma region BookingInvoiceLine
    const char *BookingInvoiceLine::GetCreateQuerySqlite()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInvoiceLine ("
                  "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "InvoiceId BLOB, "
                  "UnqiueId BLOB, "
                  "BookingFeatureId BLOB, "
                  "Count REAL, "
                  "LineTotal REAL"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetCreateQueryPostgreSQL()
    {
        static std::string Query;
        std::ostringstream Stream;

        Stream << "CREATE TABLE IF NOT EXISTS BookingInvoiceLine ("
                  "Id SERIAL PRIMARY KEY, "
                  "InvoiceId UUID, "
                  "UnqiueId UUID, "
                  "BookingFeatureId UUID, "
                  "Count DOUBLE PRECISION, "
                  "LineTotal DOUBLE PRECISION"
                  ")";

        Query = Stream.str();
        return Query.c_str();
    }

    const char *BookingInvoiceLine::GetInsertQuerySqlite()
    {
        return "INSERT INTO BookingInvoiceLine "
               "(InvoiceId, UnqiueId, BookingFeatureId, Count, LineTotal) "
               "VALUES (?, ?, ?, ?, ?)";
    }

    const char *BookingInvoiceLine::GetCountQuery()
    {
        return "SELECT COUNT(*) FROM BookingInvoiceLine";
    }

    const char *BookingInvoiceLine::GetByIdQuery()
    {
        return "SELECT "
               "Id, "
               "InvoiceId, "
               "UnqiueId, "
               "BookingFeatureId, "
               "Count, "
               "LineTotal "
               "FROM BookingInvoiceLine "
               "WHERE Id = ?";
    }

    const char *BookingInvoiceLine::GetName()
    {
        return "BookingInvoiceLine";
    }
#pragma endregion

} // namespace NepBill
