#ifndef NEPBILL_TABLES_H
#define NEPBILL_TABLES_H

#include <stdint.h>
#include <ctime> // Import the ctime library
#include <array>
#include "UUID.h"

namespace NepBill
{
    static constexpr uint32_t kDescription = 256;
    static constexpr uint32_t kNameLength = 64;
    static constexpr uint32_t ItemNameLength = 256;
    static constexpr uint32_t kPanNumberLength = 32;
    static constexpr uint32_t BusinessInfoVatNumberLength = 64;
    static constexpr uint32_t BusinessInfoAddressLength = 64;
    static constexpr uint32_t kPaymentTicketNotes = 256;
    static constexpr uint32_t ExpensesNotes = 256;
    static constexpr uint32_t kPhoneNumberLength = 16;

    // main identifier and only one per application  instance
    struct BusinessInfo
    {
        uint32_t Id = 0;
        std::array<char, kNameLength> Name = {0};
        std::array<char, BusinessInfoVatNumberLength> VatNumber = {0};
        std::array<char, BusinessInfoAddressLength> Address;
        std::array<char, kPanNumberLength> PanNumber;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        struct tm FiscalYearStart;
        UUID UniqueID;

        static const char *GetCreateQuerySqlite();
        static const char *GetCreateQueryPostgreSQL();
        static const char *GetInsertQuerySqlite();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct Expenses
    {
        uint32_t Id = 0;
        UUID CategoryId;
        uint32_t CategoryIdAtBs = 0;
        double Amount = 0;
        std::array<char, ExpensesNotes> Note;
        uint32_t SupplierId = 0;
    };

    struct TaxPeriods
    {
        uint32_t Id = 0;
        uint32_t PeriodStartBs = 0;
        uint32_t PeriodEndBs = 0;
        double VatCollected = 0;
        double VatPaid = 0;
        uint32_t Filed = 0;
    };

    // ---- Customer Related ----

    struct Customer
    {
        uint32_t Id = 0;
        UUID UniqueID;
        std::array<char, kPhoneNumberLength> PhoneNumber;
        std::array<char, kNameLength> Name;

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
        Customer,
        // Amount paid to the business owner
        Business,
        // Charge paid to the product owner various issues
        ServiceCharge,
        // Charge paid to the supplier
        Supplier
    };

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

    enum class PaymentTicketType
    {
        Cash,
        Esewa,
        Khalti,
        Bank
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
        std::array<char, kPaymentTicketNotes> Note;

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
        std::array<char, kDescription> Description;

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
        uint32_t StockDelta = 0;
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

} // namespace NepBill

#endif
