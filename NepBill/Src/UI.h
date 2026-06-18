#ifndef NEPBILL_UI_H
#define NEPBILL_UI_H

#include <memory>
#include <optional>
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/Statement.h"
#include "Tables.h"
#include "NepBill.h"
#include "Data.h"
#include "NepaliDateConverter.h"

namespace NepBill
{
    namespace ClientCpp
    {
        enum class DashboardSection
        {
            Dashboard,
            Rooms,
            Bookings,
            Suppliers,
            PaymentTickets,
            Items,
            Invoices,
            Staffs
        };

        std::string TmToStr(const tm &Time);

        struct UIDashboardInfo
        {
            uint32_t ActiveBookings = 0;
            uint32_t BookingsCountWeek = 0;
            uint32_t BookingsCountMonth = 0;
        };

        enum class RoomSection
        {
            None,
            RegisterRoom,
            ViewRoomFacility,
            EditRoom
        };

        enum class RoomFacilitySection
        {
            None,
            RegisterRoomFacility,
            EditRoomFacility
        };

        struct UIRoomFacilityInfo
        {
            RoomFacilityInfo New, Edit;
            std::vector<RoomFacilityInfo> Show;
            bool UpdateList = true;
            RoomFacilitySection Section = RoomFacilitySection::None;
            uint32_t LastRoomId;
            bool ShowWindow = true;
        };

        struct UIRoomInfo
        {
            struct RoomExtraInfo
            {
                double RoomPrice;
            };

            RoomInfo NewRoom;
            RoomInfo EditRoom;
            RoomInfo FacilityActiveRoom;
            UIRoomFacilityInfo Facility;
            std::vector<RoomInfo> ShowRooms;
            std::vector<RoomExtraInfo> ShowExtraInfo;
            bool UpdateList = true;
            RoomSection Section = RoomSection::None;
            RoomSection LastSection = RoomSection::None;
        };

        enum class BookingInfoSection
        {
            None,
            CreateNewBooking,
            EditBooking,
            CheckInBooking,
            CheckOutBooking,
            ViewBookingAdditionalCost,
        };

        enum class BookingAdditionalInfoSection
        {
            None,
            AddAdditionalCost,
            EditAdditionalCost
        };

        struct UIBookingAdditionalCostInfo
        {
            BookingAdditionalCostInfo New;
            BookingAdditionalCostInfo Edit;
            std::vector<BookingAdditionalCostInfo> Show;
            std::vector<Item> ExtraItemInfo;
            std::vector<Item> ActiveItems;
            BookingAdditionalInfoSection Section = BookingAdditionalInfoSection::None;
            bool UpdateList = true;
            uint32_t LastId = UINT32_MAX;
            bool ShowWindow = true;
        };

        struct UIBookingInfo
        {
            BookingInfo New;
            BookingInfo Edit;
            std::vector<BookingInfo> Show;
            // All rooms of bookings
            std::vector<RoomInfo> BookingRooms;
            uint32_t NewBookingGuestCount = 0;
            std::vector<GuestInfo> BookingGuests;
            std::vector<RoomInfo> FreeRooms;
            std::vector<std::string> FreeRoomInfo;
            bool UpdateList = true;
            BookingInfoSection Section = BookingInfoSection::None;
            BookingInfoSection LastSection = BookingInfoSection::None;
            UIBookingAdditionalCostInfo AdditionalCost;
        };

        struct UInvoiceInfo
        {
            std::vector<Invoice> Show;
            bool UpdateList = true;
        };

        enum class ItemSections
        {
            None,
            RegisterItem,
            ViewCategory,
            EditItem,
            ViewStock
        };

        enum class ItemCategoriesSections
        {
            None,
            RegisterCategory,
        };

        struct UItemCategory
        {
            ItemCategory New;
            std::vector<ItemCategory> Show;
            ItemCategoriesSections Section = ItemCategoriesSections::None;
            bool UpdateList = true;
            uint32_t LastId = UINT32_MAX;
            bool ShowWindow = true;
        };

        struct UItem
        {
            struct ItemMember
            {
                Item Item;
                ItemCategory Category;
            };

            Item New, Edit;
            std::vector<ItemMember> Show;
            UItemCategory Category;
            bool UpdateList = true;
            ItemSections Section = ItemSections::None;
            ItemSections LastSection = ItemSections::None;
        };

        struct UIApp
        {
            DashboardSection Section = DashboardSection::Dashboard;
            DashboardSection LastSection = DashboardSection::Dashboard;
            UIDashboardInfo Dashboard;
            UIBookingInfo Booking;
            UIRoomInfo Room;
            UItem Item;
            std::string ErrorMessage;
            UInvoiceInfo Invoice;
        };

        void UIDashboard(UIApp &UI, App &App);
        void UIRoomSection(UIApp &UI, App &App);
        bool UIRoomFacilitySection(UIApp &UI, App &App, RoomInfo &ActiveRoom);
        void UIBookingSection(UIApp &UI, App &App);
        bool UIBookingAdditionalCostSection(UIApp &UI, App &App, BookingInfo &Info);
        void UInvoiceection(UIApp &UI, App &App);
        void UItemSection(UIApp &UI, App &App);
        bool UItemCategorySection(UIApp &UI, App &App);
    } // namespace ClientCpp
} // namespace NepBill

#endif
