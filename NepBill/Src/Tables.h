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
    static constexpr uint32_t kPasswordLength = 64;
    static constexpr uint32_t kSaltBytesLength = 16;
    static constexpr uint32_t kNonceBytesLength = 24;
    static constexpr uint32_t kContactMessageLength = 2048;
    static constexpr uint32_t kSecretKeySize = 32;

    enum class ContactSubjectType
    {
        RegisterAccount,
        Issue,
        Help,
        Billing,
        Other
    };

    enum class AccountIdType
    {
        None,
        PhoneNumber,
        Email
    };

    // Convert Enum to String
    constexpr const char *ContactSubjectTypeToStr(ContactSubjectType role) noexcept
    {
        switch (role)
        {
        case ContactSubjectType::RegisterAccount:
        {
            return "RegisterAccount";
        }
        case ContactSubjectType::Issue:
        {
            return "Issue";
        }
        case ContactSubjectType::Help:
        {
            return "Help";
        }
        case ContactSubjectType::Billing:
        {
            return "Billing";
        }
        case ContactSubjectType::Other:
        {
            return "Other";
        }
        default:
            return "Unknown";
        }
    }

    // Convert String to Enum
    constexpr std::optional<ContactSubjectType> StrToContactSubjectType(const char *role_str) noexcept
    {
        if (role_str == nullptr)
        {
            return std::nullopt;
        }

        const std::string_view str(role_str);

        if (str == "RegisterAccount")
        {
            return ContactSubjectType::RegisterAccount;
        }
        if (str == "Issue")
        {
            return ContactSubjectType::Issue;
        }
        if (str == "Help")
        {
            return ContactSubjectType::Help;
        }
        if (str == "Billing")
        {
            return ContactSubjectType::Billing;
        }
        if (str == "Other")
        {
            return ContactSubjectType::Other;
        }

        return std::nullopt; // Returns empty if string matches nothing
    }

    enum class ContactStates
    {
        None,
        Processing,
        Handled,
        Failed
    };

    // Convert enum to string
    inline std::string ContactStatesToStr(ContactStates state)
    {
        switch (state)
        {
        case ContactStates::None:
            return "None";
        case ContactStates::Processing:
            return "Processing";
        case ContactStates::Handled:
            return "Handled";
        case ContactStates::Failed:
            return "Failed";
        }
        return "Unknown";
    }

    // Convert string to enum (using std::optional for safety)
    inline std::optional<ContactStates> StrToContactStates(const std::string &str)
    {
        if (str == "None")
            return ContactStates::None;
        if (str == "Processing")
            return ContactStates::Processing;
        if (str == "Handled")
            return ContactStates::Handled;
        if (str == "Failed")
            return ContactStates::Failed;
        return std::nullopt; // Invalid string case
    }

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

    // Convert Enum to String
    constexpr const char *AccountRolesToStr(AccountRoles role) noexcept
    {
        switch (role)
        {
        case AccountRoles::None:
            return "None";
        case AccountRoles::BusinessOwner:
            return "BusinessOwner";
        case AccountRoles::BusinessManager:
            return "BusinessManager";
        case AccountRoles::BusinessReceptionist:
            return "BusinessReceptionist";
        case AccountRoles::BusinessAccountant:
            return "BusinessAccountant";
        case AccountRoles::BusinessInventorManager:
            return "BusinessInventorManager";
        case AccountRoles::BusinessStaff:
            return "BusinessStaff";
        case AccountRoles::BusinessKitchen:
            return "BusinessKitchen";
        case AccountRoles::Admin:
            return "Admin";
        default:
            return "Unknown";
        }
    }

    // Convert String to Enum
    constexpr std::optional<AccountRoles> StrToAccountRoles(const char *role_str) noexcept
    {
        if (role_str == "None")
            return AccountRoles::None;
        if (role_str == "BusinessOwner")
            return AccountRoles::BusinessOwner;
        if (role_str == "BusinessManager")
            return AccountRoles::BusinessManager;
        if (role_str == "BusinessReceptionist")
            return AccountRoles::BusinessReceptionist;
        if (role_str == "BusinessAccountant")
            return AccountRoles::BusinessAccountant;
        if (role_str == "BusinessInventorManager")
            return AccountRoles::BusinessInventorManager;
        if (role_str == "BusinessStaff")
            return AccountRoles::BusinessStaff;
        if (role_str == "BusinessKitchen")
            return AccountRoles::BusinessKitchen;
        if (role_str == "Admin")
            return AccountRoles::Admin;
        return std::nullopt; // Returns empty if string matches nothing
    }

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

    struct PdfDocument
    {
        uint32_t Id = 0;
        UUID UniqueId;
        std::array<char, kNameLength> Name;
        std::vector<uint8_t> Data; // Holds the raw PDF binary stream

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct Image
    {
        uint32_t Id = 0;
        UUID UniqueId;
        std::array<char, kNameLength> Name;
        std::vector<uint8_t> Data; // Manages binary data and stores size automatically

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
        std::array<char, kNameLength> Ownername = {0};
        std::array<char, kPhoneNumberLength> PrimaryPhoneNumber = {0}; // owner's phone, for contact/billing
        std::array<char, kVatNumberLength> VatNumber = {0};
        std::array<char, kPanNumberLength> PanNumber = {0};
        std::array<char, kCountryNameLength> Country = {0};
        std::array<char, kAddressLength> Address = {0};
        std::array<char, kPasswordLength> Password = {0};
        UUID VatCertificatePdf;
        UUID PanPdf;
        UUID BusinessFrontImage;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct Account
    {
        uint32_t Id = 0;
        UUID UniqueId;
        UUID BusinessID;    // which business this account belongs to
        UUID ContactFormID; // the RegisterAccount submission that created this account
        UUID ParentID = UUID::InvalidUUID();
        std::array<char, kPasswordHashLength> PasswordHash = {0};
        AccountRoles Role = AccountRoles::None;
        bool IsActive = true;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct EncryptedSecret
    {
        UUID AccountID; // Links back to Account.UniqueId
        std::array<unsigned char, kSaltBytesLength> KeySalt = {0};
        std::array<unsigned char, kNonceBytesLength> EncryptionNonce = {0};
        std::array<unsigned char, kPasswordHashLength> Ciphertext = {0};
        std::size_t ActualCiphertextSize = 0;

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

    struct FeaturePermission
    {
        uint32_t Id = 0;
        UUID UniqueId;
        UUID BusinessID;
        AccountRoles Role;
        std::array<char, kNameLength> FeatureKey = {0}; // e.g. "view_salary", "cancel_booking"
        bool IsAllowed = true;

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
        UUID RegisterAccountFormId;
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
        enum ServiceType ServiceType;
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

    enum class StockMovementReason
    {
        PurchaseDelivery, // came from a supplier via PO
        BookingUsage,     // used for a guest booking charge
        ManualAdjustment, // manual correction — spoilage, breakage, count error
        StockTake,        // periodic physical count correction
    };

    struct ItemStockLedger
    {
        uint32_t Id = 0;
        UUID BusinessID;
        UUID PurchaseOrderID = UUID::InvalidUUID(); // set if stock came from a PO delivery
        UUID ItemId;
        int32_t StockDelta = 0;
        UUID AccountID;             // who recorded this movement
        StockMovementReason Reason; // enum below
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

    enum class PurchaseOrderState
    {
        Draft,
        Sent,
        PartiallyReceived,
        Completed
    };

    // Enum → string
    inline constexpr std::string PurchaseOrderStateToStr(PurchaseOrderState state)
    {
        switch (state)
        {
        case PurchaseOrderState::Draft:
            return "Draft";
        case PurchaseOrderState::Sent:
            return "Sent";
        case PurchaseOrderState::PartiallyReceived:
            return "PartiallyReceived";
        case PurchaseOrderState::Completed:
            return "Completed";
        }
        return "Unknown";
    }

    // string → Enum
    inline PurchaseOrderState StrToPurchaseOrderState(const std::string &str)
    {
        static const std::unordered_map<std::string_view, PurchaseOrderState> map = {
            {"Draft", PurchaseOrderState::Draft},
            {"Sent", PurchaseOrderState::Sent},
            {"PartiallyReceived", PurchaseOrderState::PartiallyReceived},
            {"Completed", PurchaseOrderState::Completed},
        };

        auto it = map.find(str);
        if (it == map.end())
            throw std::invalid_argument("Unknown PurchaseOrderState: " + std::string(str));
        return it->second;
    }

    struct PurchaseOrder
    {
        uint32_t Id = 0;
        UUID UniqueID;
        UUID BusinessID;
        UUID SupplierID; // remove ItemId from here
        UUID ItemInvoiceId = UUID::InvalidUUID();
        PurchaseOrderState State = PurchaseOrderState::Draft;
        struct tm CreatedAt;

        static const char *GetCreateQuery();
        static const char *GetInsertQuery();
        static const char *GetCountQuery();
        static const char *GetByIdQuery();
        static const char *GetName();
    };

    struct PurchaseOrderLine
    {
        uint32_t Id = 0;
        UUID UniqueID;
        UUID PurchaseOrderID;
        UUID ItemID;
        uint32_t OrderedQuantity = 0;
        uint32_t ReceivedQuantity = 0; // updated as deliveries happen
        double UnitPrice = 0.0;
        double DiscountPercent = 0.0;

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