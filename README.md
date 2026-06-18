# NepBill

C++ based offline-first SME billing &amp; inventory, hotel managment application, A native Windows desktop app (C++, SQLite, Dear ImGui or a thin Win32 UI). Runs 100% offline. Syncs to a local network or USB when internet exists. No subscription needed for core features.

## System Flow

<p align="center">
    <img src="Docs/Images/FlowChart.png" alt="NepBill Flow Chart" width="900"/>
</p>

## Tables

## Business Info

The core of the business only one instance per business their PanNumber, PhoneNumber,
Address, VatNumber, Name

```cpp
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
```

### Customers Related

## Customer

Stores basic info realted to the customer, the customer phone number and history of purchases on his compleition rate of payment is public

```cpp

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
```

### Payment Related

## Invoice

## Invoice States

```cpp
    enum InvoiceStates : uint32_t
    {
        Paying,
        PartiallyPaid,
        FullyPaid,
        Dropped,
    };
```

A simple payment invoice that can be from customer to business or from business to supplier where the invoice can be in states of paying, partiallypaid, Fullypaid and dropped based on the circumstances contains the vat percent directed to the whole invoice and its lines

```cpp
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
```

## Invoice Line

A bill or invoice in simple terms targeting a specific item and the ordererd quantity and delivered quantity for this item

```cpp
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
```

## Payment Ticket

# Payment Ticket Type

```cpp
    enum class PaymentTicketType
    {
        Cash,
        Esewa,
        Khalti,
        Bank
    };
```

The main payment data in our application where it can only be invoked on paying for a invoice with a set amount paid at a time with various payment methods such as cash or online including the type of person sending and recieving such that we can find out if this was a customer purchase or a supplier purchase payment

```cpp
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

```

### Items Related

## Item Category

A simple string identifier for a item category such as Books, Electronics,etc... useful for faster recognition of items

```cpp
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
```

## Item

Contains info about a item such as its unique category for faster identification the cost price and the sales price differing on the rate of discount on them, the cost and sales price are per unit, Name to identify this item and a simple description

```cpp
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
```

## Item Stock Ledger

On calculating item stock we dont have a global per item stock count instead rather we have a list of stock ledgers having the stock delta and we calculate the total sum to find out the current stock

```cpp
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

```

## Suppliers

To identify and store information about the suppliers simply stores their pan and phone number with name and the balance

```cpp
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
```
