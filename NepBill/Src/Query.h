#ifndef NEPBILL_QUERY_H
#define NEPBILL_QUERY_H

#include <optional>
#include "Tables.h"

namespace NepBill
{
    struct Pagination
    {
        std::optional<uint32_t> Limit;
        uint32_t Offset = 0;
    };
    // ---- ContactFormStateMetaInfo ----
    enum class ContactStateMetaSortField
    {
        Id,
        FormID
    };

    struct ContactFormStateMetaInfoQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> FormID;

        struct Pagination Pagination;
        ContactStateMetaSortField SortField = ContactStateMetaSortField::Id;
        bool SortDescending = true;
    };

    // ---- ContactFormInfo ----
    enum class ContactSortField
    {
        CreatedAt,
        PhoneNumber
    };

    struct ContactFormQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<std::string> PhoneNumber;
        std::optional<std::string> Email;
        std::optional<ContactSubjectType> Type;
        std::optional<ContactStates> State;

        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        struct Pagination Pagination;
        ContactSortField SortField = ContactSortField::CreatedAt;
        bool SortDescending = true;
    };

    enum class RegisterAccountContactSortField
    {
        Id,
        Name,
        Ownername,
        Country
    };

    struct RegisterAccountContactFormQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> ContactID;
        std::optional<std::string> Name;
        std::optional<std::string> Ownername;
        std::optional<std::string> PrimaryPhoneNumber;
        std::optional<std::string> VatNumber;
        std::optional<std::string> PanNumber;
        std::optional<std::string> Country;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        RegisterAccountContactSortField SortField = RegisterAccountContactSortField::Id;
        bool SortDescending = true;
    };

    enum class PdfDocumentSortField
    {
        Id,
        Name
    };

    struct PdfDocumentQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<std::string> Name;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        PdfDocumentSortField SortField = PdfDocumentSortField::Id;
        bool SortDescending = true;
    };

    enum class ImageSortField
    {
        Id,
        Name
    };

    struct ImageQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<std::string> Name;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        ImageSortField SortField = ImageSortField::Id;
        bool SortDescending = true;
    };

    enum class BusinessSortField
    {
        Id,
        UniqueId,
        RegisterAccountFormId,
        CreatedAt
    };

    struct BusinessQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<UUID> RegisterAccountFormId;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        BusinessSortField SortField = BusinessSortField::Id;
        bool SortDescending = true;
    };

    enum class AccountSortField
    {
        Id,
        UniqueId,
        BusinessID,
        ContactFormID,
        Role,
        IsActive
    };

    struct AccountQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<UUID> BusinessID;
        std::optional<UUID> ContactFormID;
        std::optional<UUID> ParentID;
        std::optional<AccountRoles> Role;
        std::optional<bool> IsActive;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        AccountSortField SortField = AccountSortField::Id;
        bool SortDescending = true;
    };

    enum class EncryptedSecretSortField
    {
        AccountID,
        ActualCiphertextSize
    };

    struct EncryptedSecretQuery
    {
        // ---- Filters ----
        std::optional<UUID> AccountID;

        // ---- Pagination ----
        struct Pagination Pagination;

        // ---- Sorting ----
        EncryptedSecretSortField SortField = EncryptedSecretSortField::AccountID;
        bool SortDescending = true;
    };

    // Update query
    struct UpdateContactFormQuery
    {
        // Identifier to specify which record to update
        std::optional<UUID> UniqueId;
        std::optional<uint32_t> Id;

        // Fields to update (only non-null fields will be updated)
        std::optional<std::string> PhoneNumber;
        std::optional<std::string> Email;
        std::optional<std::string> Message;
        std::optional<ContactSubjectType> Type;
        std::optional<ContactStates> State;
    };

    enum class ItemStockLedgerSortField
    {
        Id,
        CreatedAt,
        StockDelta
    };

    struct ItemStockLedgerQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> BusinessID;
        std::optional<UUID> PurchaseOrderID;
        std::optional<UUID> ItemId;
        std::optional<UUID> AccountID;
        std::optional<StockMovementReason> Reason;

        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        struct Pagination Pagination;
        ItemStockLedgerSortField SortField = ItemStockLedgerSortField::CreatedAt;
        bool SortDescending = true;
    };

    // ---- PurchaseOrder Query ----
    enum class PurchaseOrderSortField
    {
        Id,
        CreatedAt
    };

    struct PurchaseOrderQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueID;
        std::optional<UUID> BusinessID;
        std::optional<UUID> SupplierID;
        std::optional<UUID> ItemInvoiceId;
        std::optional<PurchaseOrderState> State;

        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        struct Pagination Pagination;
        PurchaseOrderSortField SortField = PurchaseOrderSortField::CreatedAt;
        bool SortDescending = true;
    };

    // ---- PurchaseOrderLine Query ----
    enum class PurchaseOrderLineSortField
    {
        Id,
        OrderedQuantity,
        UnitPrice
    };

    struct PurchaseOrderLineQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueID;
        std::optional<UUID> PurchaseOrderID;
        std::optional<UUID> ItemID;

        struct Pagination Pagination;
        PurchaseOrderLineSortField SortField = PurchaseOrderLineSortField::Id;
        bool SortDescending = false;
    };

    // ---- ItemInvoice Query ----
    enum class ItemInvoiceSortField
    {
        Id,
        CreatedAt,
        LineTotal
    };

    // Add service type filters to ItemInvoiceQuery struct
    struct ItemInvoiceQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> SenderId;
        std::optional<UUID> RecieverId;
        std::optional<UUID> UniqueID;
        std::optional<InvoiceStates> State;
        std::optional<FinancialPaymentReceipientType> SenderType;
        std::optional<ServiceType> SenderServiceType;
        std::optional<FinancialPaymentReceipientType> RecieverType;
        std::optional<ServiceType> RecieverServiceType;

        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        struct Pagination Pagination;
        ItemInvoiceSortField SortField = ItemInvoiceSortField::CreatedAt;
        bool SortDescending = true;
    };

    // ---- ItemInvoiceLine Query ----
    enum class ItemInvoiceLineSortField
    {
        Id,
        OrderedQuantity,
        UnitPrice,
        LineTotal
    };

    struct ItemInvoiceLineQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> InvoiceId;
        std::optional<UUID> ItemId;
        std::optional<UUID> UnqiueId;

        struct Pagination Pagination;
        ItemInvoiceLineSortField SortField = ItemInvoiceLineSortField::Id;
        bool SortDescending = false;
    };

    enum class SuppliersSortField
    {
        Id,
        Name,
        OpeningBalance
    };

    struct SuppliersQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> BusinessID;
        std::optional<UUID> UnqiueId;
        std::optional<std::string> Name;
        std::optional<std::string> PhoneNumber;
        std::optional<std::string> PanNumber;

        struct Pagination Pagination;
        SuppliersSortField SortField = SuppliersSortField::Id;
        bool SortDescending = false;
    };

    enum class ItemCategorySortField
    {
        Id,
        Name
    };

    struct ItemCategoryQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> BusinessID;
        std::optional<UUID> UniqueID;
        std::optional<std::string> Name;

        struct Pagination Pagination;
        ItemCategorySortField SortField = ItemCategorySortField::Id;
        bool SortDescending = false;
    };

    // ---- Item Query ----
    enum class ItemSortField
    {
        Id,
        Name,
        CostPrice,
        SalesPrice,
        LowStockThresold
    };

    struct ItemQuery
    {
        std::optional<uint32_t> Id;
        std::optional<UUID> BusinessID;
        std::optional<UUID> CategoryId;
        std::optional<UUID> UniqueID;
        std::optional<std::string> Name;

        struct Pagination Pagination;
        ItemSortField SortField = ItemSortField::Id;
        bool SortDescending = false;
    };
}
#endif