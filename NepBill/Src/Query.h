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

    enum class ContactSortField
    {
        CreatedAt,
        PhoneNumber
    };

    struct ContactFormQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> UniqueId;
        std::optional<std::string> PhoneNumber;
        std::optional<std::string> Email;
        std::optional<ContactSubjectType> Type;
        std::optional<ContactStates> State;

        // ---- Date filters ----
        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        ContactSortField SortField = ContactSortField::CreatedAt;
        bool SortDescending = true;
    };

    enum class RegisterAccountSortField
    {
        Name,
        PrimaryPhoneNumber
    };

    struct RegisterAccountContactFormQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> ContactID;
        std::optional<std::string> Name;
        std::optional<std::string> PrimaryPhoneNumber;
        std::optional<std::string> VatNumber;
        std::optional<std::string> PanNumber;
        std::optional<std::string> Country;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        RegisterAccountSortField SortField =
            RegisterAccountSortField::Name;

        bool SortDescending = false;
    };

    enum class AccountRoleSortField
    {
        Id,
        AccountID,
        Role
    };

    struct AccountRoleQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> AccountID;
        std::optional<AccountRoles> Role;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        AccountRoleSortField SortField = AccountRoleSortField::Id;
        bool SortDescending = false;
    };

    enum class ContactFormStateMetaSortField
    {
        Id,
        FormID
    };

    struct ContactFormStateMetaQuery
    {
        // ---- Filters ----
        std::optional<uint32_t> Id;
        std::optional<UUID> FormID;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        ContactFormStateMetaSortField SortField = ContactFormStateMetaSortField::Id;
        bool SortDescending = false;
    };
}
#endif