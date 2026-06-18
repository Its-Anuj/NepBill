#ifndef NEPBILL_H
#define NEPBILL_H

#include <memory>
#include <optional>
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/Statement.h"
#include "Tables.h"
#include "Query.h"

namespace NepBill
{
    namespace ClientCpp
    {
        struct App
        {
            std::unique_ptr<SQLite::Database> Database;
            HotelInfo Info;
            std::string Name;
        };

        inline int64_t ToUnixTime(const std::tm &Time)
        {
            std::tm Copy = Time;
            return static_cast<int64_t>(std::mktime(&Copy));
        }

        inline std::tm GetCurrentTime()
        {
            std::time_t Now = std::time(nullptr);
            return *std::localtime(&Now);
        }

        enum class DatabaseAskReturnCodes
        {
            HotelRowNotPresent,
            TablesNotPresent,
            NoAsk
        };

        void InitDatabase(App &App, const std::string &Name);
        void SetupDatabase(App &App);
        DatabaseAskReturnCodes ShouldBusinessRowAsk(App &App);

        inline void BindField(SQLite::Statement &Query,
                              int Index,
                              const UUID &Value)
        {
            Query.bind(Index, Value.Data(), 16);
        }

        inline void BindField(SQLite::Statement &Query,
                              int Index,
                              const std::tm &Value)
        {
            Query.bind(Index, ToUnixTime(Value));
        }

        inline void Bind(SQLite::Statement &Query,
                         const HotelInfo &Object)
        {
            Query.bind(1, Object.Name.data());
            Query.bind(2, Object.VatNumber.data());
            Query.bind(3, Object.Address.data());
            Query.bind(4, Object.PanNumber.data());
            Query.bind(5, Object.PhoneNumber.data());
            Query.bind(6, Object.Country.data());

            BindField(Query, 7, Object.UniqueID);
        }

        inline void Bind(SQLite::Statement &Query,
                         const RoomFacilityInfo &Object)
        {
            BindField(Query, 1, Object.UniqueID);
            BindField(Query, 2, Object.RoomID);

            Query.bind(
                3,
                Object.FacilityInfo.data());

            Query.bind(
                4,
                Object.FacilityPrice);
        }

        inline void Bind(SQLite::Statement &Query,
                         const RoomInfo &Object)
        {
            int Index = 1;

            Query.bind(
                Index++,
                Object.Name.data());

            Query.bind(
                Index++,
                Object.BasePrice);

            Query.bind(
                Index++,
                static_cast<int>(Object.BedCount));

            Query.bind(
                Index++,
                Object.Description.data());

            BindField(
                Query,
                Index++,
                Object.UniqueID);

            Query.bind(
                Index++,
                static_cast<int>(Object.State));
        }

        inline void Bind(SQLite::Statement &Query, const Invoice &Obj)
        {
            int i = 1;

            BindField(Query, i++, Obj.SenderId);
            BindField(Query, i++, Obj.RecieverId);
            BindField(Query, i++, Obj.UniqueID);

            Query.bind(i++, static_cast<int>(Obj.State));
            Query.bind(i++, Obj.VatPercent);
            Query.bind(i++, Obj.LineTotal);

            Query.bind(i++, static_cast<int>(Obj.SenderType));
            Query.bind(i++, static_cast<int>(Obj.RecieverType));

            BindField(Query, i++, Obj.CreatedAt);
            BindField(Query, i++, Obj.LastPaymentTicketDate);
            BindField(Query, i++, Obj.ClosedAt);
        }

        inline void Bind(SQLite::Statement &Query, const InvoiceLine &Object)
        {
            int idx = 1;

            BindField(Query, idx++, Object.InvoiceId);
            BindField(Query, idx++, Object.ItemId);
            BindField(Query, idx++, Object.UnqiueId);

            Query.bind(idx++, Object.OrderedQuantity);
            Query.bind(idx++, Object.DeliveredQuantity);

            Query.bind(idx++, Object.ItemWeight);
            Query.bind(idx++, Object.UnitPrice);
            Query.bind(idx++, Object.UnitDiscountPercet);
            Query.bind(idx++, Object.LineTotal);
        }

        inline void Bind(SQLite::Statement &Query, const PaymentTicket &Object)
        {
            int idx = 1;

            Query.bind(idx++, Object.PaidAmount);

            Query.bind(idx++, static_cast<int>(Object.Sender));
            Query.bind(idx++, static_cast<int>(Object.Receiver));

            BindField(Query, idx++, Object.InvoiceID);
            BindField(Query, idx++, Object.UniqueID);

            Query.bind(idx++, static_cast<int>(Object.PaidType));

            Query.bind(idx++, static_cast<long long>(ToUnixTime(Object.CreatedAt)));

            Query.bind(idx++, Object.Note.data());
        }

        inline void Bind(SQLite::Statement &Query, const ItemCategory &Object)
        {
            int idx = 1;

            BindField(Query, idx++, Object.UniqueID);
            Query.bind(idx++, Object.Name.data());
        }

        inline void Bind(SQLite::Statement &Query, const BookingInfo &Obj)
        {
            int i = 1;

            BindField(Query, i++, Obj.UniqueID);
            BindField(Query, i++, Obj.InvoiceID);
            BindField(Query, i++, Obj.RoomID);

            BindField(Query, i++, Obj.CreatedAt);
            BindField(Query, i++, Obj.CheckIn);
            BindField(Query, i++, Obj.CheckOut);
        }

        inline void Bind(SQLite::Statement &Query,
                         const GuestInfo &Object)
        {
            int Index = 1;

            BindField(Query, Index++, Object.UniqueID);

            Query.bind(Index++, Object.Name.data());
            Query.bind(Index++, Object.PhoneNumber.data());
            Query.bind(Index++, Object.Country.data());
        }

        inline void Bind(SQLite::Statement &Query, const Item &Object)
        {
            int idx = 1;

            BindField(Query, idx++, Object.CategoryId);
            BindField(Query, idx++, Object.UniqueID);

            Query.bind(idx++, Object.Name.data());
            Query.bind(idx++, Object.LowStockThresold);

            Query.bind(idx++, Object.CostPrice);
            Query.bind(idx++, Object.SalesPrice);
            Query.bind(idx++, Object.DiscountPercent);

            Query.bind(idx++, Object.Description.data());
        }

        inline void Bind(SQLite::Statement &Query, const ItemStockLedger &Object)
        {
            int idx = 1;

            BindField(Query, idx++, Object.ItemId);
            Query.bind(idx++, Object.StockDelta);

            Query.bind(idx++, static_cast<long long>(ToUnixTime(Object.CreatedAt)));
        }

        inline void Bind(SQLite::Statement &Query, const Suppliers &Object)
        {
            int idx = 1;

            BindField(Query, idx++, Object.UnqiueId);

            Query.bind(idx++, Object.Name.data());
            Query.bind(idx++, Object.PhoneNumber.data());
            Query.bind(idx++, Object.PanNumber.data());

            Query.bind(idx++, Object.OpeningBalance);
        }

        inline void Bind(SQLite::Statement &Query,
                         const BookingGuestTable &Object)
        {
            int Index = 1;

            BindField(Query, Index++, Object.GuestID);
            BindField(Query, Index++, Object.BookingID);
        }

        inline void Bind(SQLite::Statement &Query,
                         const StaffSalaryPayment &Object)
        {
            int i = 1;

            BindField(Query, i++, Object.UnqiueID);
            BindField(Query, i++, Object.StaffID);

            Query.bind(i++, Object.Amount);

            BindField(Query, i++, Object.Date);

            Query.bind(i++, Object.Description.data());
        }

        inline void Bind(SQLite::Statement &Query,
                         const BookingInvoice &Object)
        {
            BindField(Query, 1, Object.GuestId);
            BindField(Query, 2, Object.UniqueID);

            Query.bind(
                3,
                static_cast<int>(Object.State));

            Query.bind(
                4,
                Object.VatPercent);

            Query.bind(
                5,
                Object.LineTotal);

            BindField(
                Query,
                6,
                Object.CreatedAt);

            BindField(
                Query,
                7,
                Object.LastPaymentTicketDate);

            BindField(
                Query,
                8,
                Object.ClosedAt);
        }

        inline void Insert(
            App &App,
            const BookingAdditionalCostInfo &Info)
        {
            SQLite::Statement Statement(
                *App.Database,
                BookingAdditionalCostInfo::GetInsertQuerySqlite());

            int BindIndex = 1;

            BindField(
                Statement,
                BindIndex++,
                Info.UnqiueID);

            BindField(
                Statement,
                BindIndex++,
                Info.BookingID);

            BindField(
                Statement,
                BindIndex++,
                Info.ItemID);

            Statement.bind(
                BindIndex++,
                Info.PricePerUnit);

            Statement.bind(
                BindIndex++,
                static_cast<int>(Info.UnitCount));

            Statement.exec();
        }

        inline void Bind(SQLite::Statement &Query,
                         const BookingInvoiceLine &Object)
        {
            BindField(
                Query,
                1,
                Object.InvoiceId);

            BindField(
                Query,
                2,
                Object.UnqiueId);

            BindField(
                Query,
                3,
                Object.BookingFeatureId);

            Query.bind(
                4,
                Object.Count);

            Query.bind(
                5,
                Object.LineTotal);
        }

        template <typename T>
        void Insert(App &App, const T &Object)
        {
            SQLite::Statement Query(
                *App.Database,
                T::GetInsertQuerySqlite());

            Bind(Query, Object);

            Query.exec();
        }

        std::optional<HotelInfo> QueryHotelInfo(App &App);

        std::vector<BookingInfo> GetBookings(App &App, const BookingQuery &Q);
        std::vector<BookingGuestTable> GetBookingGuests(App &App, const BookingGuestQuery &QueryInfo);
        std::vector<GuestInfo> GetGuests(App &App, const GuestQuery &QueryInfo);
        std::vector<Invoice> GetInvoices(App &App, const InvoiceQuery &Q);
        std::vector<InvoiceLine> GetInvoiceLines(App &App, const InvoiceLineQuery &Q);
        std::vector<BookingInvoiceLine> GetBookingInvoiceLines(App &App, const BookingInvoiceLineQuery &QueryInfo);
        std::vector<BookingInvoice> GetBookingInvoices(App &App, const BookingInvoiceQuery &QueryInfo);
        std::vector<RoomFacilityInfo> GetRoomFacilities(App &App, const RoomFacilityQuery &QueryInfo);
        std::vector<BookingAdditionalCostInfo> GetBookingAdditionalCosts(App &App, const BookingAdditionalCostQuery &QueryInfo);
        std::vector<ItemCategory> GetItemCategories(App &App, const ItemCategoryQuery &QueryInfo);
        std::vector<Item> GetItems(App &App, const ItemQuery &QueryInfo);
        std::vector<RoomInfo> GetRooms(App &App, const RoomQuery &QueryInfo);

        void UpdateRoomInfo(App &App,
                            uint32_t ChangeRoomID,
                            const RoomInfo &UpdateInfo);
        void UpdateBookingInfo(App &App,
                               uint32_t ChangeBookingInfo,
                               const BookingInfo &UpdateInfo);
        void UpdateRoomFacilityInfo(App &App,
                                    uint32_t ChangeFacilityID,
                                    const RoomFacilityInfo &UpdateInfo);
    } // namespace Client
} // namespace NepBill

#endif