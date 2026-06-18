#ifndef NEPBILL_SHARED_QUERY_H
#define NEPBILL_SHARED_QUERY_H

#include <optional>
#include "Tables.h"

namespace NepBill
{
    struct Pagination
    {
        std::optional<uint32_t> Limit;
        uint32_t Offset = 0;
    };

    enum class RoomSortField
    {
        Name,
        BasePrice,
        BedCount,
        State
    };

    struct RoomQuery
    {
        // Filters

        std::optional<UUID> UniqueID;

        std::optional<RoomStates> State;

        std::optional<uint32_t> MinBeds;
        std::optional<uint32_t> MaxBeds;

        std::optional<double> MinPrice;
        std::optional<double> MaxPrice;

        // Search

        std::optional<std::string> NameContains;

        // Pagination

        uint32_t Limit = 25;
        uint32_t Offset = 0;

        // Sorting

        RoomSortField SortField =
            RoomSortField::Name;

        bool SortDescending = false;
    };

    struct BookingInvoiceLineQuery
    {
        std::optional<UUID> InvoiceId;
        std::optional<UUID> UnqiueId;
        std::optional<UUID> BookingFeatureId;

        std::optional<double> MinCount;
        std::optional<double> MaxCount;

        Pagination Page;

        bool SortDescending = false;
    };

    struct BookingInvoiceQuery
    {
        std::optional<UUID> UniqueID;
        std::optional<UUID> GuestId;

        std::optional<InvoiceStates> State;

        std::optional<double> MinTotal;
        std::optional<double> MaxTotal;

        std::optional<double> MinVat;
        std::optional<double> MaxVat;

        Pagination Page;

        bool SortDescending = false;
    };

    enum class BookingSortField
    {
        CreatedAt,
        CheckIn,
        CheckOut
    };

    struct BookingQuery
    {
        // ---- Filters ----
        std::optional<UUID> BookingId;
        std::optional<UUID> GuestId;
        std::optional<UUID> RoomId;
        std::optional<UUID> InvoiceId;

        // ---- Date filters ----
        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        std::optional<time_t> CheckInAfter;
        std::optional<time_t> CheckInBefore;

        std::optional<time_t> CheckOutAfter;
        std::optional<time_t> CheckOutBefore;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        BookingSortField SortField = BookingSortField::CreatedAt;
        bool SortDescending = true;
    };

    enum class InvoiceSortField
    {
        CreatedAt,
        LastPaymentDate,
        ClosedAt,
        LineTotal
    };

    struct InvoiceQuery
    {
        // ---- Filters ----
        std::optional<UUID> InvoiceId;

        std::optional<UUID> SenderId;
        std::optional<UUID> ReceiverId;

        std::optional<FinancialPaymentReceipientType> SenderType;
        std::optional<FinancialPaymentReceipientType> ReceiverType;

        std::optional<InvoiceStates> State;

        // ---- Amount filters ----
        std::optional<double> MinTotal;
        std::optional<double> MaxTotal;

        // ---- Date filters ----
        std::optional<time_t> CreatedAfter;
        std::optional<time_t> CreatedBefore;

        std::optional<time_t> LastPaidAfter;
        std::optional<time_t> LastPaidBefore;

        std::optional<time_t> ClosedAfter;
        std::optional<time_t> ClosedBefore;

        // ---- Pagination ----
        Pagination Pagination;

        // ---- Sorting ----
        InvoiceSortField SortField = InvoiceSortField::CreatedAt;
        bool SortDescending = true;
    };

    struct InvoiceLineQuery
    {
        std::optional<UUID> InvoiceId;
        std::optional<UUID> ItemId;
        std::optional<UUID> UniqueId;

        std::optional<uint32_t> MinOrdered;
        std::optional<uint32_t> MaxOrdered;

        std::optional<double> MinTotal;
        std::optional<double> MaxTotal;

        Pagination Page;
    };

    struct GuestQuery
    {
        std::optional<UUID> UniqueID;

        std::optional<std::string> NameContains;
        std::optional<std::string> PhoneContains;
        std::optional<std::string> CountryContains;

        Pagination Page;
    };

    struct BookingGuestQuery
    {
        std::optional<UUID> GuestID;
        std::optional<UUID> BookingID;

        Pagination Page;
    };

    struct RoomFacilityQuery
    {
        // Filters

        std::optional<UUID> UniqueID;
        std::optional<UUID> RoomID;

        std::optional<double> MinPrice;
        std::optional<double> MaxPrice;

        std::optional<std::string> FacilityContains;

        Pagination Page;

        bool SortDescending = false;

        enum class SortField
        {
            FacilityInfo,
            FacilityPrice
        };

        SortField SortBy =
            SortField::FacilityInfo;
    };

    struct BookingAdditionalCostQuery
    {
        std::optional<UUID> UniqueID;
        std::optional<UUID> BookingID;
        std::optional<UUID> ItemID;

        Pagination Page;
    };

    struct ItemCategoryQuery
    {
        std::optional<UUID> UniqueID;
        std::optional<std::string> NameContains;

        Pagination Page;
    };

    struct ItemQuery
    {
        std::optional<UUID> UniqueID;
        std::optional<UUID> CategoryId;

        std::optional<std::string> NameContains;

        std::optional<double> MinCostPrice;
        std::optional<double> MaxCostPrice;

        std::optional<double> MinSalesPrice;
        std::optional<double> MaxSalesPrice;

        std::optional<uint32_t> MinLowStockThreshold;
        std::optional<uint32_t> MaxLowStockThreshold;

        Pagination Page;
    };
} // namespace NepBil

#endif