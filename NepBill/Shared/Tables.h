#ifndef NEPBILL_SHARED_TABLES_H
#define NEPBILL_SHARED_TABLES_H

#include <array>
#include "UUID.h"
#include <ctime> // Import the ctime library
#include <stdint.h>

namespace NepBill
{
    static constexpr uint32_t kNameLength = 64;
    static constexpr uint32_t kPhoneNumberLength = 16;
    static constexpr uint32_t kPanNumberLength = 32;
    static constexpr uint32_t kAddressLength = 64;
    static constexpr uint32_t kVatNumberLength = 64;
    static constexpr uint32_t kPaymentTicketNotesLength = 256;
    static constexpr uint32_t ItemNameLength = 256;
    static constexpr uint32_t ItemDescriptionLength = 512;
    static constexpr uint32_t RoomFacilityLength = 128;
    static constexpr uint32_t RoomDescriptionLength = 256;
    static constexpr uint32_t SalaryPaymentDescriptionLength = 256;
    static constexpr uint32_t kCountryNameLength = 128;

    // ---- Hotel Info ----

    struct HotelInfo
    {
        uint32_t Id = 0;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kVatNumberLength> VatNumber = {0};
        std::array<char, kAddressLength> Address;
        std::array<char, kPanNumberLength> PanNumber;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        std::array<char, kCountryNameLength> Country;
        UUID UniqueID;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Payment Related ----

    enum InvoiceStates : uint32_t
    {
        Paying,
        PartiallyPaid,
        FullyPaid,
        Dropped,
    };

    enum FinancialPaymentReceipientType : uint32_t
    {
        // Amount paid to the customer maybe return who knows
        Guest,
        // Amount paid to the business owner
        OurBusiness,
        // Charge paid to the product owner various issues
        ServiceCharge,
        // Charge paid to the supplier
        Supplier
    };

    static constexpr const char *ReceipientTypeStr[] = {
        "Guest",
        "OurBusiness",
        "ServiceCharge",
        "Supplier"};

    // Single invoice can have multiple tickets
    struct Invoice
    {
        uint32_t Id = 0;
        UUID SenderId;
        UUID RecieverId;
        UUID UniqueID;
        InvoiceStates State;
        double VatPercent = 0;
        double LineTotal = 0;
        FinancialPaymentReceipientType SenderType;
        FinancialPaymentReceipientType RecieverType;

        struct tm CreatedAt;
        // Date specifing the last time a payment ticket was opened for this invoice
        struct tm LastPaymentTicketDate;
        // Closed date on either dropped or fully paid condition
        struct tm ClosedAt;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // One invoie can be related to multiple item purchases
    struct InvoiceLine
    {
        uint32_t Id = 0;
        UUID InvoiceId;
        UUID ItemId;
        UUID UnqiueId;
        uint32_t OrderedQuantity;
        uint32_t DeliveredQuantity;
        double ItemWeight;
        double UnitPrice;
        double UnitDiscountPercet;
        double LineTotal;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // Single invoice can have multiple tickets
    struct BookingInvoice
    {
        uint32_t Id = 0;
        UUID GuestId;
        UUID UniqueID;
        InvoiceStates State;
        double VatPercent = 0;
        double LineTotal = 0;

        struct tm CreatedAt;
        // Date specifing the last time a payment ticket was opened for this invoice
        struct tm LastPaymentTicketDate;
        // Closed date on either dropped or fully paid condition
        struct tm ClosedAt;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // One invoie can be related to multiple item purchases
    struct BookingInvoiceLine
    {
        uint32_t Id = 0;
        UUID InvoiceId;
        UUID UnqiueId;
        UUID BookingFeatureId;
        double Count;
        double LineTotal;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    enum class PaymentTicketType
    {
        Cash,
        Esewa,
        Khalti,
        Bank
    };

    enum class InvoiceTargetType : uint32_t
    {
        ItemInvoice,
        BookingInvoice
    };

    struct PaymentTicket
    {
        uint32_t Id = 0;
        double PaidAmount = 0;
        FinancialPaymentReceipientType Sender;
        FinancialPaymentReceipientType Receiver;
        UUID InvoiceID;
        UUID UniqueID;
        // Futurre Add like rrrreceiveer u know esewa id or such stuff idk
        PaymentTicketType PaidType;
        struct tm CreatedAt;
        std::array<char, kPaymentTicketNotesLength> Note;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Item ---

    struct ItemCategory
    {
        uint32_t Id = 0;
        UUID UniqueID;
        std::array<char, kNameLength> Name;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct Item
    {
        uint32_t Id = 0;
        UUID CategoryId;
        UUID UniqueID;
        std::array<char, ItemNameLength> Name;
        uint32_t LowStockThresold = 0;
        double CostPrice = 0;
        double SalesPrice = 0;
        double DiscountPercent = 0;
        std::array<char, ItemDescriptionLength> Description;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct ItemStockLedger
    {
        uint32_t Id = 0;
        UUID ItemId;
        int32_t StockDelta = 0;
        struct tm CreatedAt;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Supplier Related ----

    struct Suppliers
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        std::array<char, kNameLength> Name;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        std::array<char, kPanNumberLength> PanNumber;
        double OpeningBalance;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Room Related ----

    enum class RoomStates : uint32_t
    {
        // available to be rented
        Available,
        // specifically only for cases where guest is staying
        Taken,
        // Can be set due to work or other issues
        Unavailable,
        Count
    };

    constexpr const char *RoomStateStr[] = {
        "Available",
        "Taken",
        "Unavailable"};

    struct RoomInfo
    {
        uint32_t Id = 0;
        std::array<char, kNameLength> Name;
        double BasePrice = 0.0f;
        uint32_t BedCount = 0;
        std::array<char, RoomDescriptionLength> Description;
        UUID UniqueID;
        RoomStates State;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // Can be wifi, toilet, swimmig pool etc
    struct RoomFacilityInfo
    {
        uint32_t Id = 0;
        UUID UniqueID;
        UUID RoomID;
        std::array<char, RoomFacilityLength> FacilityInfo;
        double FacilityPrice = 0.0f;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Guest Related ----

    struct GuestInfo
    {
        uint32_t Id = 0;
        UUID UniqueID;
        std::array<char, kNameLength> Name;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        std::array<char, kCountryNameLength> Country;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Bookings ----

    struct BookingInfo
    {
        uint32_t Id = 0;
        UUID UniqueID;
        UUID InvoiceID;
        UUID RoomID;

        struct tm CreatedAt;

        struct tm CheckIn;
        struct tm CheckOut;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct BookingGuestTable
    {
        uint32_t Id = 0;
        UUID GuestID;
        UUID BookingID;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct BookingAdditionalCostInfo
    {
        uint32_t Id = 0;
        UUID UnqiueID;
        UUID BookingID;
        UUID ItemID;
        double PricePerUnit = 0;
        uint32_t UnitCount = 0;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Staff related ----

    struct StaffInfo
    {
        uint32_t Id = 0;
        UUID UnqiueID;
        std::array<char, kNameLength> Name;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        std::array<char, kPhoneNumberLength> EmergencyPhoneNumber;
        bool IsActive = true;
        double SalaryPerDay;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct StaffLedger
    {
        uint32_t Id = 0;
        UUID StaffID;
        struct tm CheckIn;
        struct tm CheckOut;
        bool PaymentRelated = false;
        UUID SalaryPaymentID;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct StaffSalaryPayment
    {
        uint32_t Id = 0;
        UUID UnqiueID;
        UUID StaffID;
        double Amount = 0.0;
        struct tm Date;
        std::array<char, SalaryPaymentDescriptionLength> Description;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

} // namespace NepBill

#endif