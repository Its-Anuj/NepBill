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

        Pagination Pagination;
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

        Pagination Pagination;
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
        Pagination Pagination;

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
        Pagination Pagination;

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
        Pagination Pagination;

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
        Pagination Pagination;

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
        Pagination Pagination;

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
        Pagination Pagination;

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
}
#endif