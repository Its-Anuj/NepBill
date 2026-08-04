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
    static constexpr uint32_t kEmailLength = 128;
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
    static constexpr uint32_t kPasswordHashLength = 128;
    static constexpr uint32_t kContactMessageLength = 2048;

    enum class ContactSubjectType
    {
        RegisterAccount,
        Issue,
        Help,
        Billing,
        Other
    };

    enum class ContactStates
    {
        None,
        Processing,
        Handled,
        Failed
    };

    enum class AccountRoles : uint32_t
    {
        None,
        BusinessOwner,
        BusinessManager,
        BusinessReceptionist,
        BusinessAccountant,
        BusinessInventorManager,
        BusinessStaff,
        BusinessKitchen,
        Admin,
    };

    struct ContactFormInfo
    {
        uint32_t Id = 0;
        UUID UniqueId;
        std::array<char, kPhoneNumberLength> PhoneNumber = {0}; // owner's phone, for contact/billing
        std::array<char, kEmailLength> Email = {0};             // owner's phone, for contact/billing
        std::array<char, kContactMessageLength> Message = {0};  // owner's phone, for contact/billing
        ContactSubjectType Type;
        struct tm CreatedAt;
        ContactStates State = ContactStates::None;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct ContactFormStateMetaInfo
    {
        uint32_t Id = 0;
        UUID FormID;
        std::array<char, kContactMessageLength> StateMessage = {0}; // owner's phone, for contact/billing

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct RegisterAccountContactFormInfo
    {
        uint32_t Id = 0;
        UUID ContactID;

        std::array<char, kNameLength> Name = {0};
        std::array<char, kPhoneNumberLength> PrimaryPhoneNumber = {0}; // owner's phone, for contact/billing
        std::array<char, kVatNumberLength> VatNumber = {0};
        std::array<char, kPanNumberLength> PanNumber = {0};
        std::array<char, kCountryNameLength> Country = {0};
        std::array<char, kAddressLength> Address = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct AccountRoleInfo
    {
        uint32_t Id = 0;
        UUID AccountID;
        AccountRoles Role = AccountRoles::None;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    enum BillingUsage : uint32_t
    {
        Hotel = 1,
        Restaurant = 2
    };

    // ---- Hotel Info ----

    struct Business
    {
        uint32_t Id = 0;
        UUID UniqueId;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kPhoneNumberLength> PrimaryPhoneNumber = {0}; // owner's phone, for contact/billing
        std::array<char, kVatNumberLength> VatNumber = {0};
        std::array<char, kPanNumberLength> PanNumber = {0};
        std::array<char, kCountryNameLength> Country = {0};
        std::array<char, kAddressLength> Address = {0};
        UUID ParentID = UUID::InvalidUUID();
        struct tm CreatedAt;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    enum class BusinessLoginRole : uint32_t
    {
        Owner,
        Manager,
        Staff
    };

    enum class ServiceType : uint32_t
    {
        Hotel = 1,
        Restaurant = 2,
        Hostel = 4
    };

    struct BusinessSubscription
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        UUID BusinessID;
        uint32_t Service = 0; // Hotel, Restaurant, Hostel...
        struct tm StartedAt;
        struct tm ExpiresAt;
        bool IsActive = true;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // This will function as a businesslogin history table too so to store from where, at what time, who logined to which
    // account for security
    struct BusinessLogin
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        UUID BusinessID;
        std::array<char, kPhoneNumberLength> PhoneNumber = {0}; // login credential
        std::array<char, kPasswordHashLength> PasswordHash = {0};
        BusinessLoginRole Role; // Owner, Manager, Staff
        bool IsActive = true;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct HotelBusiness
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        UUID BusinessID;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kAddressLength> Address = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct HostelBusiness
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        UUID BusinessID;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kAddressLength> Address = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct RestaurantBusiness
    {
        uint32_t Id = 0;
        UUID UnqiueId;
        UUID BusinessID;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kAddressLength> Address = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
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

    struct FinancialPaymentReceiptientInfo
    {
        FinancialPaymentReceipientType Type;
        ServiceType ServiceType;
    };

    static constexpr const char *ReceipientTypeStr[] = {
        "Guest",
        "OurBusiness",
        "ServiceCharge",
        "Supplier"};

    // Single invoice can have multiple tickets
    struct ItemInvoice
    {
        uint32_t Id = 0;
        UUID SenderId;
        UUID RecieverId;
        UUID UniqueID;
        InvoiceStates State;
        double VatPercent = 0;
        double LineTotal = 0;
        FinancialPaymentReceiptientInfo SenderType;
        FinancialPaymentReceiptientInfo RecieverType;

        struct tm CreatedAt;
        // Date specifing the last time a payment ticket was opened for this invoice
        struct tm LastPaymentTicketDate;
        // Closed date on either dropped or fully paid condition
        struct tm ClosedAt;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // One invoie can be related to multiple item purchases
    struct ItemInvoiceLine
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

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // Single invoice can have multiple tickets
    struct BookingInvoice
    {
        uint32_t Id = 0;
        UUID GuestId;
        UUID HotelBusinessID;
        UUID UniqueID;
        InvoiceStates State;
        double VatPercent = 0;
        double LineTotal = 0;

        struct tm CreatedAt;
        // Date specifing the last time a payment ticket was opened for this invoice
        struct tm LastPaymentTicketDate;
        // Closed date on either dropped or fully paid condition
        struct tm ClosedAt;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
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

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
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
        BookingInvoice,
    };

    struct PaymentTicket
    {
        uint32_t Id = 0;
        double PaidAmount = 0;
        FinancialPaymentReceiptientInfo SenderType;
        FinancialPaymentReceiptientInfo RecieverType;
        UUID InvoiceID;
        UUID UniqueID;
        // Futurre Add like rrrreceiveer u know esewa id or such stuff idk
        PaymentTicketType PaidType;
        struct tm CreatedAt;
        std::array<char, kPaymentTicketNotesLength> Note = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Item ---

    struct ItemCategory
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID UniqueID;
        std::array<char, kNameLength> Name = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct Item
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID CategoryId;
        UUID UniqueID;
        std::array<char, ItemNameLength> Name = {0};
        uint32_t LowStockThresold = 0;
        double CostPrice = 0;
        double SalesPrice = 0;
        double DiscountPercent = 0;
        std::array<char, ItemDescriptionLength> Description = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct ItemStockLedger
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID ItemId;
        int32_t StockDelta = 0;
        struct tm CreatedAt;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Supplier Related ----

    struct Suppliers
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID UnqiueId;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kPhoneNumberLength> PhoneNumber = {0};
        std::array<char, kPanNumberLength> PanNumber = {0};
        double OpeningBalance;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // relates a specific supplier to the item they supply
    struct SuppliersItemTable
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID SupplierID;
        UUID ItemID;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
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

    // Only given for Hostel and Hotel businesses
    struct RoomInfo
    {
        uint32_t Id = 0;
        std::array<char, kNameLength> Name = {0};
        double BasePrice = 0.0f;
        uint32_t BedCount = 0;
        std::array<char, RoomDescriptionLength> Description = {0};
        UUID UniqueID;
        RoomStates State;
        UUID BusinessID;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
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
        UUID BusinessID;
        std::array<char, RoomFacilityLength> FacilityInfo = {0};
        double FacilityPrice = 0.0f;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Guest Related ----

    // For restaurant, hotel and hostels
    struct GuestInfo
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID UniqueID;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kPhoneNumberLength> PhoneNumber = {0};
        std::array<char, kCountryNameLength> Country = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Bookings ----

    // only for Hotels
    struct HotelBookingInfo
    {
        uint32_t Id = 0;
        UUID UniqueID;
        UUID InvoiceID;
        UUID RoomID;
        UUID HotelBusinessID;

        struct tm CreatedAt;

        struct tm CheckIn;
        struct tm CheckOut;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct HotelBookingGuestTable
    {
        uint32_t Id = 0;
        UUID HotelBusinessID;
        UUID GuestID;
        UUID BookingID;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct HotelBookingAdditionalCostInfo
    {
        uint32_t Id = 0;
        UUID HotelBusinessID;
        UUID UnqiueID;
        UUID BookingID;
        UUID ItemID;
        double PricePerUnit = 0;
        uint32_t UnitCount = 0;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    // ---- Staff related ----

    // For all restaruants, hotels and hostels
    struct StaffInfo
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID UnqiueID;
        std::array<char, kNameLength> Name = {0};
        std::array<char, kPhoneNumberLength> PhoneNumber = {0};
        std::array<char, kPhoneNumberLength> EmergencyPhoneNumber = {0};
        bool IsActive = true;
        double SalaryPerDay;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct StaffLedger
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID StaffID;
        struct tm CheckIn;
        struct tm CheckOut;
        bool PaymentRelated = false;
        UUID SalaryPaymentID;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct StaffSalaryPayment
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID UnqiueID;
        UUID StaffID;
        double Amount = 0.0;
        struct tm Date;
        std::array<char, SalaryPaymentDescriptionLength> Description = {0};

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

} // namespace NepBill

#endif