#include <iostream>
#include <sstream>
#include "NepBill.h"

namespace NepBill
{
    namespace ClientCpp
    {
        void InitDatabase(App &App, const std::string &Name)
        {
            App.Database = std::make_unique<SQLite::Database>(Name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            App.Name = Name;
        }

        void SetupDatabase(App &App)
        {
            auto Db = App.Database.get();

            try
            {
                Db->exec(HotelInfo::GetCreateQuerySqlite());
                Db->exec(Invoice::GetCreateQuerySqlite());
                Db->exec(InvoiceLine::GetCreateQuerySqlite());
                Db->exec(PaymentTicket::GetCreateQuerySqlite());
                Db->exec(ItemCategory::GetCreateQuerySqlite());
                Db->exec(Item::GetCreateQuerySqlite());
                Db->exec(ItemStockLedger::GetCreateQuerySqlite());
                Db->exec(Suppliers::GetCreateQuerySqlite());
                Db->exec(RoomInfo::GetCreateQuerySqlite());
                Db->exec(RoomFacilityInfo::GetCreateQuerySqlite());
                Db->exec(BookingInfo::GetCreateQuerySqlite());
                Db->exec(BookingGuestTable::GetCreateQuerySqlite());
                Db->exec(BookingAdditionalCostInfo::GetCreateQuerySqlite());
                Db->exec(StaffInfo::GetCreateQuerySqlite());
                Db->exec(StaffLedger::GetCreateQuerySqlite());
                Db->exec(StaffSalaryPayment::GetCreateQuerySqlite());
                Db->exec(GuestInfo::GetCreateQuerySqlite());
            }
            catch (const SQLite::Exception &e)
            {
                std::cerr
                    << "SQLite Error: "
                    << e.what()
                    << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr
                    << "Standard Error: "
                    << e.what()
                    << std::endl;
            }
        }

        DatabaseAskReturnCodes ShouldBusinessRowAsk(App &App)
        {
            // Check first if all tables exist
            auto Db = App.Database.get();
            SQLite::Statement Query(*Db, "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name;");
            std::vector<std::string> ActiveTableNames;
            // Loop through the results
            while (Query.executeStep())
            {
                std::string tableName = Query.getColumn(0);
                ActiveTableNames.push_back(tableName);
            }

            std::vector<std::string> RequiredTables = {
                HotelInfo::GetName(),
                Invoice::GetName(),
                InvoiceLine::GetName(),
                PaymentTicket::GetName(),
                ItemCategory::GetName(),
                Item::GetName(),
                ItemStockLedger::GetName(),
                Suppliers::GetName(),
                RoomInfo::GetName(),
                RoomFacilityInfo::GetName(),
                BookingInfo::GetName(),
                BookingGuestTable::GetName(),
                BookingAdditionalCostInfo::GetName(),
                StaffInfo::GetName(),
                StaffLedger::GetName(),
                StaffSalaryPayment::GetName(),
                GuestInfo::GetName()};

            for (const auto &required : RequiredTables)
            {
                if (std::find(ActiveTableNames.begin(),
                              ActiveTableNames.end(),
                              required) == ActiveTableNames.end())
                {
                    return DatabaseAskReturnCodes::TablesNotPresent; // missing table found
                }
            }

            // Check if we have a existing business info row?
            // if yes we return false because a business info account already exists
            // if no we allw to ask

            try
            {
                auto Info = QueryHotelInfo(App);
                if (Info == std::nullopt)
                    return DatabaseAskReturnCodes::HotelRowNotPresent;

                App.Info = Info.value();

                return DatabaseAskReturnCodes::NoAsk;
            }
            catch (const SQLite::Exception &e)
            {
                std::cerr
                    << "SQLite Error: "
                    << e.what()
                    << std::endl;
                return DatabaseAskReturnCodes::HotelRowNotPresent;
            }
            catch (const std::exception &e)
            {
                std::cerr
                    << "Standard Error: "
                    << e.what()
                    << std::endl;
                return DatabaseAskReturnCodes::HotelRowNotPresent;
            }

            return DatabaseAskReturnCodes::HotelRowNotPresent;
        }

        std::optional<HotelInfo> QueryHotelInfo(App &App)
        {
            SQLite::Statement Query(
                *App.Database,
                HotelInfo::GetByIdQuery());

            Query.bind(1, 1); // assuming single-row config table

            if (!Query.executeStep())
                return std::nullopt;

            HotelInfo Info;
            Info.Id = Query.getColumn("Id").getUInt();

            strncpy(
                Info.Name.data(),
                Query.getColumn("Name").getString().c_str(),
                Info.Name.size());

            strncpy(
                Info.VatNumber.data(),
                Query.getColumn("VatNumber").getString().c_str(),
                Info.VatNumber.size());

            strncpy(
                Info.Address.data(),
                Query.getColumn("Address").getString().c_str(),
                Info.Address.size());

            strncpy(
                Info.PanNumber.data(),
                Query.getColumn("PanNumber").getString().c_str(),
                Info.PanNumber.size());

            strncpy(
                Info.PhoneNumber.data(),
                Query.getColumn("PhoneNumber").getString().c_str(),
                Info.PhoneNumber.size());

            strncpy(
                Info.Country.data(),
                Query.getColumn("Country").getString().c_str(),
                Info.Country.size());

            std::memcpy(
                (void *)Info.UniqueID.Data(),
                Query.getColumn("UniqueID").getBlob(),
                16);

            return Info;
        }

        std::vector<RoomInfo> GetRooms(App &App, const RoomQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM RoomInfo WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.State)
                Sql << "AND State = ? ";

            if (QueryInfo.MinBeds)
                Sql << "AND BedCount >= ? ";

            if (QueryInfo.MaxBeds)
                Sql << "AND BedCount <= ? ";

            if (QueryInfo.MinPrice)
                Sql << "AND BasePrice >= ? ";

            if (QueryInfo.MaxPrice)
                Sql << "AND BasePrice <= ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            Sql << "ORDER BY ";

            switch (QueryInfo.SortField)
            {
            case RoomSortField::Name:
                Sql << "Name";
                break;

            case RoomSortField::BasePrice:
                Sql << "BasePrice";
                break;

            case RoomSortField::BedCount:
                Sql << "BedCount";
                break;

            case RoomSortField::State:
                Sql << "State";
                break;
            }

            Sql << (QueryInfo.SortDescending ? " DESC " : " ASC ");

            Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);

            if (QueryInfo.State)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.State));

            if (QueryInfo.MinBeds)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.MinBeds));

            if (QueryInfo.MaxBeds)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.MaxBeds));

            if (QueryInfo.MinPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinPrice);

            if (QueryInfo.MaxPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxPrice);

            if (QueryInfo.NameContains)
            {
                std::string Search =
                    "%" + *QueryInfo.NameContains + "%";

                Statement.bind(
                    BindIndex++,
                    Search);
            }

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Limit));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Offset));

            std::vector<RoomInfo> Results;

            while (Statement.executeStep())
            {
                RoomInfo Room;

                Room.Id =
                    Statement.getColumn("Id").getUInt();

                std::strncpy(
                    Room.Name.data(),
                    Statement.getColumn("Name")
                        .getString()
                        .c_str(),
                    Room.Name.size());

                Room.BasePrice =
                    Statement.getColumn("BasePrice")
                        .getDouble();

                Room.BedCount =
                    Statement.getColumn("BedCount")
                        .getUInt();

                std::strncpy(
                    Room.Description.data(),
                    Statement.getColumn("Description")
                        .getString()
                        .c_str(),
                    Room.Description.size());

                std::memcpy(
                    (void *)Room.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                Room.State =
                    static_cast<RoomStates>(
                        Statement.getColumn("State")
                            .getUInt());

                Results.emplace_back(
                    std::move(Room));
            }

            return Results;
        }

        void UpdateRoomInfo(App &App,
                            uint32_t ChangeRoomID,
                            const RoomInfo &UpdateInfo)
        {
            SQLite::Statement Query(
                *App.Database,
                "UPDATE RoomInfo SET "
                "Name = ?, "
                "BasePrice = ?, "
                "BedCount = ?, "
                "Description = ?, "
                "UniqueID = ?, "
                "State = ? "
                "WHERE Id = ?");

            Query.bind(1, UpdateInfo.Name.data());
            Query.bind(2, UpdateInfo.BasePrice);
            Query.bind(3, static_cast<int>(UpdateInfo.BedCount));
            Query.bind(4, UpdateInfo.Description.data());

            BindField(Query, 5, UpdateInfo.UniqueID);

            Query.bind(
                6,
                static_cast<int>(UpdateInfo.State));

            Query.bind(
                7,
                static_cast<int>(ChangeRoomID));

            Query.exec();
        }

        void UpdateSupplier(App &App,
                            uint32_t SupplierId,
                            const Suppliers &UpdateInfo)
        {
            SQLite::Statement Query(
                *App.Database,
                "UPDATE Suppliers SET "
                "UnqiueId = ?, "
                "Name = ?, "
                "PhoneNumber = ?, "
                "PanNumber = ?, "
                "OpeningBalance = ? "
                "WHERE Id = ?");

            BindField(
                Query,
                1,
                UpdateInfo.UnqiueId);

            Query.bind(
                2,
                UpdateInfo.Name.data());

            Query.bind(
                3,
                UpdateInfo.PhoneNumber.data());

            Query.bind(
                4,
                UpdateInfo.PanNumber.data());

            Query.bind(
                5,
                UpdateInfo.OpeningBalance);

            Query.bind(
                6,
                static_cast<int>(SupplierId));

            Query.exec();
        }

        std::vector<BookingInfo> GetBookings(App &App, const BookingQuery &Q)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM BookingInfo WHERE 1=1 ";

            // ---- Filters ----
            if (Q.BookingId)
                Sql << "AND UniqueID = ? ";

            if (Q.GuestId)
                Sql << "AND GuestID = ? ";

            if (Q.RoomId)
                Sql << "AND RoomID = ? ";

            if (Q.InvoiceId)
                Sql << "AND InvoiceID = ? ";

            // ---- Date filters ----
            if (Q.CreatedAfter)
                Sql << "AND CreatedAt >= ? ";

            if (Q.CreatedBefore)
                Sql << "AND CreatedAt <= ? ";

            if (Q.CheckInAfter)
                Sql << "AND CheckIn >= ? ";

            if (Q.CheckInBefore)
                Sql << "AND CheckIn <= ? ";

            if (Q.CheckOutAfter)
                Sql << "AND CheckOut >= ? ";

            if (Q.CheckOutBefore)
                Sql << "AND CheckOut <= ? ";

            // ---- Sorting ----
            Sql << "ORDER BY ";

            switch (Q.SortField)
            {
            case BookingSortField::CreatedAt:
                Sql << "CreatedAt";
                break;

            case BookingSortField::CheckIn:
                Sql << "CheckIn";
                break;

            case BookingSortField::CheckOut:
                Sql << "CheckOut";
                break;
            }

            Sql << (Q.SortDescending ? " DESC " : " ASC ");

            // ---- Pagination ----
            if (Q.Pagination.Limit != std::nullopt)
            {
                Sql << "LIMIT ? OFFSET ?";
            }

            SQLite::Statement Statement(*App.Database, Sql.str());

            int i = 1;

            // ---- Bind filters ----
            if (Q.BookingId)
                BindField(Statement, i++, *Q.BookingId);

            if (Q.GuestId)
                BindField(Statement, i++, *Q.GuestId);

            if (Q.RoomId)
                BindField(Statement, i++, *Q.RoomId);

            if (Q.InvoiceId)
                BindField(Statement, i++, *Q.InvoiceId);

            if (Q.CreatedAfter)
                Statement.bind(i++, (long long)*Q.CreatedAfter);

            if (Q.CreatedBefore)
                Statement.bind(i++, (long long)*Q.CreatedBefore);

            if (Q.CheckInAfter)
                Statement.bind(i++, (long long)*Q.CheckInAfter);

            if (Q.CheckInBefore)
                Statement.bind(i++, (long long)*Q.CheckInBefore);

            if (Q.CheckOutAfter)
                Statement.bind(i++, (long long)*Q.CheckOutAfter);

            if (Q.CheckOutBefore)
                Statement.bind(i++, (long long)*Q.CheckOutBefore);

            // ---- pagination ----
            if (Q.Pagination.Limit != std::nullopt)
            {
                Statement.bind(i++, (int)Q.Pagination.Limit.value());
                Statement.bind(i++, (int)Q.Pagination.Offset);
            }

            // ---- fetch ----
            std::vector<BookingInfo> Result;

            while (Statement.executeStep())
            {
                BookingInfo B;

                B.Id = Statement.getColumn("Id").getUInt();

                std::memcpy((void *)B.UniqueID.Data(),
                            Statement.getColumn("UniqueID").getBlob(), 16);

                std::memcpy((void *)B.InvoiceID.Data(),
                            Statement.getColumn("InvoiceID").getBlob(), 16);

                std::memcpy((void *)B.RoomID.Data(),
                            Statement.getColumn("RoomID").getBlob(), 16);
                {
                    time_t t = static_cast<time_t>(
                        Statement.getColumn("CreatedAt").getInt64());

                    if (t != static_cast<time_t>(-1))
                    {
                        if (std::tm *Time = std::localtime(&t))
                            B.CreatedAt = *Time;
                        else
                            B.CreatedAt = {};
                    }
                    else
                    {
                        B.CreatedAt = {};
                    }
                }

                {
                    time_t t = static_cast<time_t>(
                        Statement.getColumn("CheckIn").getInt64());

                    if (t != static_cast<time_t>(-1))
                    {
                        if (std::tm *Time = std::localtime(&t))
                            B.CheckIn = *Time;
                        else
                            B.CheckIn = {};
                    }
                    else
                    {
                        B.CheckIn = {};
                    }
                }

                {
                    time_t t = static_cast<time_t>(
                        Statement.getColumn("CheckOut").getInt64());

                    if (t != static_cast<time_t>(-1))
                    {
                        if (std::tm *Time = std::localtime(&t))
                            B.CheckOut = *Time;
                        else
                            B.CheckOut = {};
                    }
                    else
                    {
                        B.CheckOut = {};
                    }
                }

                Result.emplace_back(std::move(B));
            }

            return Result;
        }

        std::vector<Invoice> GetInvoices(App &App, const InvoiceQuery &Q)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM Invoice WHERE 1=1 ";

            // ---- Filters ----
            if (Q.InvoiceId)
                Sql << "AND UniqueID = ? ";

            if (Q.SenderId)
                Sql << "AND SenderId = ? ";

            if (Q.ReceiverId)
                Sql << "AND RecieverId = ? ";

            if (Q.SenderType)
                Sql << "AND SenderType = ? ";

            if (Q.ReceiverType)
                Sql << "AND RecieverType = ? ";

            if (Q.State)
                Sql << "AND State = ? ";

            if (Q.MinTotal)
                Sql << "AND LineTotal >= ? ";

            if (Q.MaxTotal)
                Sql << "AND LineTotal <= ? ";

            if (Q.CreatedAfter)
                Sql << "AND CreatedAt >= ? ";

            if (Q.CreatedBefore)
                Sql << "AND CreatedAt <= ? ";

            if (Q.LastPaidAfter)
                Sql << "AND LastPaymentTicketDate >= ? ";

            if (Q.LastPaidBefore)
                Sql << "AND LastPaymentTicketDate <= ? ";

            if (Q.ClosedAfter)
                Sql << "AND ClosedAt >= ? ";

            if (Q.ClosedBefore)
                Sql << "AND ClosedAt <= ? ";

            // ---- Sorting ----
            Sql << "ORDER BY ";

            switch (Q.SortField)
            {
            case InvoiceSortField::CreatedAt:
                Sql << "CreatedAt";
                break;

            case InvoiceSortField::LastPaymentDate:
                Sql << "LastPaymentTicketDate";
                break;

            case InvoiceSortField::ClosedAt:
                Sql << "ClosedAt";
                break;

            case InvoiceSortField::LineTotal:
                Sql << "LineTotal";
                break;
            }

            Sql << (Q.SortDescending ? " DESC " : " ASC ");

            // ---- Pagination ----
            if (Q.Pagination.Limit != std::nullopt)
            {
                Sql << "LIMIT ? OFFSET ?";
            }
            SQLite::Statement Statement(*App.Database, Sql.str());

            int i = 1;

            // ---- Bind filters ----
            if (Q.InvoiceId)
                BindField(Statement, i++, *Q.InvoiceId);

            if (Q.SenderId)
                BindField(Statement, i++, *Q.SenderId);

            if (Q.ReceiverId)
                BindField(Statement, i++, *Q.ReceiverId);

            if (Q.SenderType)
                Statement.bind(i++, static_cast<int>(*Q.SenderType));

            if (Q.ReceiverType)
                Statement.bind(i++, static_cast<int>(*Q.ReceiverType));

            if (Q.State)
                Statement.bind(i++, static_cast<int>(*Q.State));

            if (Q.MinTotal)
                Statement.bind(i++, *Q.MinTotal);

            if (Q.MaxTotal)
                Statement.bind(i++, *Q.MaxTotal);

            if (Q.CreatedAfter)
                Statement.bind(i++, (long long)*Q.CreatedAfter);

            if (Q.CreatedBefore)
                Statement.bind(i++, (long long)*Q.CreatedBefore);

            if (Q.LastPaidAfter)
                Statement.bind(i++, (long long)*Q.LastPaidAfter);

            if (Q.LastPaidBefore)
                Statement.bind(i++, (long long)*Q.LastPaidBefore);

            if (Q.ClosedAfter)
                Statement.bind(i++, (long long)*Q.ClosedAfter);

            if (Q.ClosedBefore)
                Statement.bind(i++, (long long)*Q.ClosedBefore);

            // ---- pagination bind ----

            // ---- Pagination ----
            if (Q.Pagination.Limit != std::nullopt)
            {
                Statement.bind(i++, (int)Q.Pagination.Limit.value());
                Statement.bind(i++, (int)Q.Pagination.Offset);
            }

            // ---- fetch ----
            std::vector<Invoice> Result;

            while (Statement.executeStep())
            {
                Invoice Inv;

                Inv.Id = Statement.getColumn("Id").getUInt();

                std::memcpy((void *)Inv.SenderId.Data(),
                            Statement.getColumn("SenderId").getBlob(), 16);

                std::memcpy((void *)Inv.RecieverId.Data(),
                            Statement.getColumn("RecieverId").getBlob(), 16);

                std::memcpy((void *)Inv.UniqueID.Data(),
                            Statement.getColumn("UniqueID").getBlob(), 16);

                Inv.State = (InvoiceStates)Statement.getColumn("State").getUInt();
                Inv.VatPercent = Statement.getColumn("VatPercent").getDouble();
                Inv.LineTotal = Statement.getColumn("LineTotal").getDouble();

                Inv.SenderType =
                    (FinancialPaymentReceipientType)Statement.getColumn("SenderType").getUInt();

                Inv.RecieverType =
                    (FinancialPaymentReceipientType)Statement.getColumn("RecieverType").getUInt();

                {
                    time_t t = (time_t)Statement.getColumn("CreatedAt").getInt64();
                    Inv.CreatedAt = *std::localtime(&t);
                }

                {
                    time_t t = (time_t)Statement.getColumn("LastPaymentTicketDate").getInt64();
                    Inv.LastPaymentTicketDate = *std::localtime(&t);
                }

                {
                    time_t t = (time_t)Statement.getColumn("ClosedAt").getInt64();
                    Inv.ClosedAt = *std::localtime(&t);
                }

                Result.emplace_back(std::move(Inv));
            }

            return Result;
        }

        std::vector<InvoiceLine> GetInvoiceLines(App &App, const InvoiceLineQuery &Q)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM InvoiceLine WHERE 1=1 ";

            if (Q.InvoiceId)
                Sql << "AND InvoiceId = ? ";

            if (Q.ItemId)
                Sql << "AND ItemId = ? ";

            if (Q.UniqueId)
                Sql << "AND UnqiueId = ? ";

            if (Q.MinOrdered)
                Sql << "AND OrderedQuantity >= ? ";

            if (Q.MaxOrdered)
                Sql << "AND OrderedQuantity <= ? ";

            if (Q.MinTotal)
                Sql << "AND LineTotal >= ? ";

            if (Q.MaxTotal)
                Sql << "AND LineTotal <= ? ";

            // ---- pagination ----
            if (Q.Page.Limit)
                Sql << "LIMIT ? OFFSET ? ";

            SQLite::Statement Stmt(*App.Database, Sql.str());

            int i = 1;

            // ---- binds ----
            if (Q.InvoiceId)
                BindField(Stmt, i++, *Q.InvoiceId);

            if (Q.ItemId)
                BindField(Stmt, i++, *Q.ItemId);

            if (Q.UniqueId)
                BindField(Stmt, i++, *Q.UniqueId);

            if (Q.MinOrdered)
                Stmt.bind(i++, (int)*Q.MinOrdered);

            if (Q.MaxOrdered)
                Stmt.bind(i++, (int)*Q.MaxOrdered);

            if (Q.MinTotal)
                Stmt.bind(i++, *Q.MinTotal);

            if (Q.MaxTotal)
                Stmt.bind(i++, *Q.MaxTotal);

            if (Q.Page.Limit)
            {
                Stmt.bind(i++, (int)*Q.Page.Limit);
                Stmt.bind(i++, (int)Q.Page.Offset);
            }

            // ---- fetch ----
            std::vector<InvoiceLine> Result;

            while (Stmt.executeStep())
            {
                InvoiceLine L;

                L.Id = Stmt.getColumn("Id").getUInt();

                std::memcpy((void *)L.InvoiceId.Data(),
                            Stmt.getColumn("InvoiceId").getBlob(), 16);

                std::memcpy((void *)L.ItemId.Data(),
                            Stmt.getColumn("ItemId").getBlob(), 16);

                std::memcpy((void *)L.UnqiueId.Data(),
                            Stmt.getColumn("UnqiueId").getBlob(), 16);

                L.OrderedQuantity = Stmt.getColumn("OrderedQuantity").getUInt();
                L.DeliveredQuantity = Stmt.getColumn("DeliveredQuantity").getUInt();

                L.ItemWeight = Stmt.getColumn("ItemWeight").getDouble();
                L.UnitPrice = Stmt.getColumn("UnitPrice").getDouble();
                L.UnitDiscountPercet = Stmt.getColumn("UnitDiscountPercet").getDouble();
                L.LineTotal = Stmt.getColumn("LineTotal").getDouble();

                Result.emplace_back(std::move(L));
            }

            return Result;
        }

        std::vector<GuestInfo>
        GetGuests(App &App,
                  const GuestQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM GuestInfo WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            if (QueryInfo.PhoneContains)
                Sql << "AND PhoneNumber LIKE ? ";

            if (QueryInfo.CountryContains)
                Sql << "AND Country LIKE ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);

            if (QueryInfo.NameContains)
                Statement.bind(
                    BindIndex++,
                    ("%" + *QueryInfo.NameContains + "%"));

            if (QueryInfo.PhoneContains)
                Statement.bind(
                    BindIndex++,
                    ("%" + *QueryInfo.PhoneContains + "%"));

            if (QueryInfo.CountryContains)
                Statement.bind(
                    BindIndex++,
                    ("%" + *QueryInfo.CountryContains + "%"));

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<GuestInfo> Results;

            while (Statement.executeStep())
            {
                GuestInfo Guest;

                Guest.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Guest.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                std::strncpy(
                    Guest.Name.data(),
                    Statement.getColumn("Name").getText(),
                    Guest.Name.size() - 1);

                std::strncpy(
                    Guest.PhoneNumber.data(),
                    Statement.getColumn("PhoneNumber").getText(),
                    Guest.PhoneNumber.size() - 1);

                std::strncpy(
                    Guest.Country.data(),
                    Statement.getColumn("Country").getText(),
                    Guest.Country.size() - 1);

                Results.emplace_back(std::move(Guest));
            }

            return Results;
        }

        std::vector<BookingGuestTable>
        GetBookingGuests(App &App,
                         const BookingGuestQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM BookingGuestTable "
                   "WHERE 1=1 ";

            if (QueryInfo.GuestID)
                Sql << "AND GuestID = ? ";

            if (QueryInfo.BookingID)
                Sql << "AND BookingID = ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.GuestID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.GuestID);

            if (QueryInfo.BookingID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.BookingID);

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<BookingGuestTable> Results;

            while (Statement.executeStep())
            {
                BookingGuestTable Entry;

                Entry.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Entry.GuestID.Data(),
                    Statement.getColumn("GuestID").getBlob(),
                    16);

                std::memcpy(
                    (void *)Entry.BookingID.Data(),
                    Statement.getColumn("BookingID").getBlob(),
                    16);

                Results.emplace_back(std::move(Entry));
            }

            return Results;
        }

        void UpdateBookingInfo(App &App,
                               uint32_t ChangeBookingID,
                               const BookingInfo &UpdateInfo)
        {
            SQLite::Statement Query(
                *App.Database,
                "UPDATE BookingInfo "
                "SET "
                "UniqueID = ?, "
                "InvoiceID = ?, "
                "RoomID = ?, "
                "CreatedAt = ?, "
                "CheckIn = ?, "
                "CheckOut = ? "
                "WHERE Id = ?");

            int BindIndex = 1;

            BindField(Query, BindIndex++, UpdateInfo.UniqueID);
            BindField(Query, BindIndex++, UpdateInfo.InvoiceID);
            BindField(Query, BindIndex++, UpdateInfo.RoomID);

            BindField(Query, BindIndex++, UpdateInfo.CreatedAt);
            BindField(Query, BindIndex++, UpdateInfo.CheckIn);
            BindField(Query, BindIndex++, UpdateInfo.CheckOut);

            Query.bind(
                BindIndex++,
                static_cast<int>(ChangeBookingID));

            Query.exec();
        }

        void UpdateRoomFacilityInfo(App &App,
                                    uint32_t ChangeFacilityID,
                                    const RoomFacilityInfo &UpdateInfo)
        {
            SQLite::Statement Query(
                *App.Database,
                "UPDATE RoomFacilityInfo "
                "SET "
                "UniqueID = ?, "
                "RoomID = ?, "
                "FacilityInfo = ?, "
                "FacilityPrice = ? "
                "WHERE Id = ?");

            int BindIndex = 1;

            BindField(
                Query,
                BindIndex++,
                UpdateInfo.UniqueID);

            BindField(
                Query,
                BindIndex++,
                UpdateInfo.RoomID);

            Query.bind(
                BindIndex++,
                UpdateInfo.FacilityInfo.data());

            Query.bind(
                BindIndex++,
                UpdateInfo.FacilityPrice);

            Query.bind(
                BindIndex++,
                static_cast<int>(ChangeFacilityID));

            Query.exec();
        }

        std::vector<BookingInvoice>
        GetBookingInvoices(App &App,
                           const BookingInvoiceQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM BookingInvoice WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.GuestId)
                Sql << "AND GuestId = ? ";

            if (QueryInfo.State)
                Sql << "AND State = ? ";

            if (QueryInfo.MinTotal)
                Sql << "AND LineTotal >= ? ";

            if (QueryInfo.MaxTotal)
                Sql << "AND LineTotal <= ? ";

            if (QueryInfo.MinVat)
                Sql << "AND VatPercent >= ? ";

            if (QueryInfo.MaxVat)
                Sql << "AND VatPercent <= ? ";

            Sql << "ORDER BY Id ";

            Sql << (QueryInfo.SortDescending ? "DESC " : "ASC ");

            if (QueryInfo.Page.Limit)
                Sql << "LIMIT ? ";

            Sql << "OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);

            if (QueryInfo.GuestId)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.GuestId);

            if (QueryInfo.State)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.State));

            if (QueryInfo.MinTotal)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinTotal);

            if (QueryInfo.MaxTotal)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxTotal);

            if (QueryInfo.MinVat)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinVat);

            if (QueryInfo.MaxVat)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxVat);

            if (QueryInfo.Page.Limit)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Page.Offset));

            std::vector<BookingInvoice> Results;

            while (Statement.executeStep())
            {
                BookingInvoice Invoice;

                Invoice.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Invoice.GuestId.Data(),
                    Statement.getColumn("GuestId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Invoice.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                Invoice.State =
                    static_cast<InvoiceStates>(
                        Statement.getColumn("State").getUInt());

                Invoice.VatPercent =
                    Statement.getColumn("VatPercent").getDouble();

                Invoice.LineTotal =
                    Statement.getColumn("LineTotal").getDouble();

                {
                    time_t t =
                        (time_t)Statement.getColumn("CreatedAt").getInt64();

                    Invoice.CreatedAt =
                        (t == -1) ? std::tm{} : *std::localtime(&t);
                }

                {
                    time_t t =
                        (time_t)Statement.getColumn("LastPaymentTicketDate").getInt64();

                    Invoice.LastPaymentTicketDate =
                        (t == -1) ? std::tm{} : *std::localtime(&t);
                }

                {
                    time_t t =
                        (time_t)Statement.getColumn("ClosedAt").getInt64();

                    Invoice.ClosedAt =
                        (t == -1) ? std::tm{} : *std::localtime(&t);
                }

                Results.emplace_back(
                    std::move(Invoice));
            }

            return Results;
        }

        std::vector<BookingInvoiceLine>
        GetBookingInvoiceLines(App &App,
                               const BookingInvoiceLineQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM BookingInvoiceLine WHERE 1=1 ";

            if (QueryInfo.InvoiceId)
                Sql << "AND InvoiceId = ? ";

            if (QueryInfo.UnqiueId)
                Sql << "AND UnqiueId = ? ";

            if (QueryInfo.BookingFeatureId)
                Sql << "AND BookingFeatureId = ? ";

            if (QueryInfo.MinCount)
                Sql << "AND Count >= ? ";

            if (QueryInfo.MaxCount)
                Sql << "AND Count <= ? ";

            Sql << "ORDER BY Id ";

            Sql << (QueryInfo.SortDescending ? "DESC " : "ASC ");

            if (QueryInfo.Page.Limit)
                Sql << "LIMIT ? ";

            Sql << "OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.InvoiceId)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.InvoiceId);

            if (QueryInfo.UnqiueId)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UnqiueId);

            if (QueryInfo.BookingFeatureId)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.BookingFeatureId);

            if (QueryInfo.MinCount)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinCount);

            if (QueryInfo.MaxCount)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxCount);

            if (QueryInfo.Page.Limit)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Page.Offset));

            std::vector<BookingInvoiceLine> Results;

            while (Statement.executeStep())
            {
                BookingInvoiceLine Line;

                Line.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Line.InvoiceId.Data(),
                    Statement.getColumn("InvoiceId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Line.UnqiueId.Data(),
                    Statement.getColumn("UnqiueId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Line.BookingFeatureId.Data(),
                    Statement.getColumn("BookingFeatureId").getBlob(),
                    16);

                Line.Count =
                    Statement.getColumn("Count").getDouble();

                Line.LineTotal =
                    Statement.getColumn("LineTotal").getDouble();

                Results.emplace_back(
                    std::move(Line));
            }

            return Results;
        }

        std::vector<RoomFacilityInfo>
        GetRoomFacilities(
            App &App,
            const RoomFacilityQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM RoomFacilityInfo "
                   "WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.RoomID)
                Sql << "AND RoomID = ? ";

            if (QueryInfo.MinPrice)
                Sql << "AND FacilityPrice >= ? ";

            if (QueryInfo.MaxPrice)
                Sql << "AND FacilityPrice <= ? ";

            if (QueryInfo.FacilityContains)
                Sql << "AND FacilityInfo LIKE ? ";

            Sql << "ORDER BY ";

            switch (QueryInfo.SortBy)
            {
            case RoomFacilityQuery::SortField::FacilityInfo:
                Sql << "FacilityInfo";
                break;

            case RoomFacilityQuery::SortField::FacilityPrice:
                Sql << "FacilityPrice";
                break;
            }

            Sql << (QueryInfo.SortDescending
                        ? " DESC "
                        : " ASC ");

            if (QueryInfo.Page.Limit)
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);
            }

            if (QueryInfo.RoomID)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.RoomID);
            }

            if (QueryInfo.MinPrice)
            {
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinPrice);
            }

            if (QueryInfo.MaxPrice)
            {
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxPrice);
            }

            if (QueryInfo.FacilityContains)
            {
                std::string Search =
                    "%" +
                    *QueryInfo.FacilityContains +
                    "%";

                Statement.bind(
                    BindIndex++,
                    Search);
            }

            if (QueryInfo.Page.Limit)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(
                        *QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(
                        QueryInfo.Page.Offset));
            }

            std::vector<RoomFacilityInfo> Results;

            while (Statement.executeStep())
            {
                RoomFacilityInfo Facility;

                Facility.Id =
                    Statement.getColumn("Id")
                        .getUInt();

                std::memcpy(
                    (void *)Facility.UniqueID.Data(),
                    Statement.getColumn("UniqueID")
                        .getBlob(),
                    16);

                std::memcpy(
                    (void *)Facility.RoomID.Data(),
                    Statement.getColumn("RoomID")
                        .getBlob(),
                    16);

                std::strncpy(
                    Facility.FacilityInfo.data(),
                    Statement.getColumn("FacilityInfo")
                        .getString()
                        .c_str(),
                    Facility.FacilityInfo.size());

                Facility.FacilityPrice =
                    Statement.getColumn("FacilityPrice")
                        .getDouble();

                Results.emplace_back(
                    std::move(Facility));
            }

            return Results;
        }
        std::vector<BookingAdditionalCostInfo>
        GetBookingAdditionalCosts(
            App &App,
            const BookingAdditionalCostQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM BookingAdditionalCostInfo "
                   "WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UnqiueID = ? ";

            if (QueryInfo.BookingID)
                Sql << "AND BookingID = ? ";

            if (QueryInfo.ItemID)
                Sql << "AND ItemID = ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);

            if (QueryInfo.BookingID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.BookingID);

            if (QueryInfo.ItemID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.ItemID);

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<BookingAdditionalCostInfo> Results;

            while (Statement.executeStep())
            {
                BookingAdditionalCostInfo Info;

                Info.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Info.UnqiueID.Data(),
                    Statement.getColumn("UnqiueID").getBlob(),
                    16);

                std::memcpy(
                    (void *)Info.BookingID.Data(),
                    Statement.getColumn("BookingID").getBlob(),
                    16);

                std::memcpy(
                    (void *)Info.ItemID.Data(),
                    Statement.getColumn("ItemID").getBlob(),
                    16);

                Info.PricePerUnit =
                    Statement.getColumn("PricePerUnit")
                        .getDouble();

                Info.UnitCount =
                    Statement.getColumn("UnitCount")
                        .getUInt();

                Results.emplace_back(
                    std::move(Info));
            }

            return Results;
        }

        std::vector<ItemCategory>
        GetItemCategories(
            App &App,
            const ItemCategoryQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM ItemCategory "
                   "WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);
            }

            if (QueryInfo.NameContains)
            {
                std::string Search =
                    "%" + *QueryInfo.NameContains + "%";

                Statement.bind(
                    BindIndex++,
                    Search);
            }

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<ItemCategory> Results;

            while (Statement.executeStep())
            {
                ItemCategory Category;

                Category.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Category.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                std::strncpy(
                    Category.Name.data(),
                    Statement.getColumn("Name")
                        .getString()
                        .c_str(),
                    Category.Name.size());

                Results.emplace_back(
                    std::move(Category));
            }

            return Results;
        }

        std::vector<Item>
        GetItems(
            App &App,
            const ItemQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM Item "
                   "WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.CategoryId)
                Sql << "AND CategoryId = ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            if (QueryInfo.MinCostPrice)
                Sql << "AND CostPrice >= ? ";

            if (QueryInfo.MaxCostPrice)
                Sql << "AND CostPrice <= ? ";

            if (QueryInfo.MinSalesPrice)
                Sql << "AND SalesPrice >= ? ";

            if (QueryInfo.MaxSalesPrice)
                Sql << "AND SalesPrice <= ? ";

            if (QueryInfo.MinLowStockThreshold)
                Sql << "AND LowStockThresold >= ? ";

            if (QueryInfo.MaxLowStockThreshold)
                Sql << "AND LowStockThresold <= ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.UniqueID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueID);

            if (QueryInfo.CategoryId)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.CategoryId);

            if (QueryInfo.NameContains)
            {
                std::string Search =
                    "%" + *QueryInfo.NameContains + "%";

                Statement.bind(
                    BindIndex++,
                    Search);
            }

            if (QueryInfo.MinCostPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinCostPrice);

            if (QueryInfo.MaxCostPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxCostPrice);

            if (QueryInfo.MinSalesPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinSalesPrice);

            if (QueryInfo.MaxSalesPrice)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxSalesPrice);

            if (QueryInfo.MinLowStockThreshold)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.MinLowStockThreshold));

            if (QueryInfo.MaxLowStockThreshold)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.MaxLowStockThreshold));

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<Item> Results;

            while (Statement.executeStep())
            {
                Item Entry;

                Entry.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Entry.CategoryId.Data(),
                    Statement.getColumn("CategoryId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Entry.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                std::strncpy(
                    Entry.Name.data(),
                    Statement.getColumn("Name")
                        .getString()
                        .c_str(),
                    Entry.Name.size());

                Entry.LowStockThresold =
                    Statement.getColumn("LowStockThresold")
                        .getUInt();

                Entry.CostPrice =
                    Statement.getColumn("CostPrice")
                        .getDouble();

                Entry.SalesPrice =
                    Statement.getColumn("SalesPrice")
                        .getDouble();

                Entry.DiscountPercent =
                    Statement.getColumn("DiscountPercent")
                        .getDouble();

                std::strncpy(
                    Entry.Description.data(),
                    Statement.getColumn("Description")
                        .getString()
                        .c_str(),
                    Entry.Description.size());

                Results.emplace_back(
                    std::move(Entry));
            }

            return Results;
        }

        std::vector<ItemStockLedger> GetItemStockLedgers(
            App &App,
            const ItemStockLedgerQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM ItemStockLedger WHERE 1=1 ";

            if (QueryInfo.ItemID)
                Sql << "AND ItemId = ? ";

            if (QueryInfo.MinStockDelta)
                Sql << "AND StockDelta >= ? ";

            if (QueryInfo.MaxStockDelta)
                Sql << "AND StockDelta <= ? ";

            Sql << "ORDER BY CreatedAt DESC ";

            if (QueryInfo.Page.Limit)
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.ItemID)
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.ItemID);

            if (QueryInfo.MinStockDelta)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MinStockDelta);

            if (QueryInfo.MaxStockDelta)
                Statement.bind(
                    BindIndex++,
                    *QueryInfo.MaxStockDelta);

            if (QueryInfo.Page.Limit)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<ItemStockLedger> Results;

            while (Statement.executeStep())
            {
                ItemStockLedger Ledger;

                Ledger.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Ledger.ItemId.Data(),
                    Statement.getColumn("ItemId").getBlob(),
                    16);

                Ledger.StockDelta =
                    Statement.getColumn("StockDelta").getInt();

                {
                    time_t T =
                        static_cast<time_t>(
                            Statement.getColumn("CreatedAt").getInt64());

                    if (T == -1)
                    {
                        Ledger.CreatedAt = {};
                    }
                    else
                    {
                        Ledger.CreatedAt =
                            *std::localtime(&T);
                    }
                }

                Results.emplace_back(
                    std::move(Ledger));
            }

            return Results;
        }

        void UpdateItem(
            App &App,
            uint32_t ChangeItemID,
            const Item &UpdateInfo)
        {
            SQLite::Statement Query(
                *App.Database,
                "UPDATE Item "
                "SET "
                "CategoryId = ?, "
                "UniqueID = ?, "
                "Name = ?, "
                "LowStockThresold = ?, "
                "CostPrice = ?, "
                "SalesPrice = ?, "
                "DiscountPercent = ?, "
                "Description = ? "
                "WHERE Id = ?");

            int BindIndex = 1;

            BindField(
                Query,
                BindIndex++,
                UpdateInfo.CategoryId);

            BindField(
                Query,
                BindIndex++,
                UpdateInfo.UniqueID);

            Query.bind(
                BindIndex++,
                UpdateInfo.Name.data());

            Query.bind(
                BindIndex++,
                static_cast<int>(UpdateInfo.LowStockThresold));

            Query.bind(
                BindIndex++,
                UpdateInfo.CostPrice);

            Query.bind(
                BindIndex++,
                UpdateInfo.SalesPrice);

            Query.bind(
                BindIndex++,
                UpdateInfo.DiscountPercent);

            Query.bind(
                BindIndex++,
                UpdateInfo.Description.data());

            Query.bind(
                BindIndex++,
                static_cast<int>(ChangeItemID));

            Query.exec();
        }

        std::vector<SuppliersItemTable>
        GetSuppliersItems(
            App &App,
            const SuppliersItemQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM SuppliersItemTable "
                   "WHERE 1=1 ";

            if (QueryInfo.Id)
                Sql << "AND Id = ? ";

            if (QueryInfo.SupplierID)
                Sql << "AND SupplierID = ? ";

            if (QueryInfo.ItemID)
                Sql << "AND ItemID = ? ";

            if (QueryInfo.Page.Limit.has_value())
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.Id)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Id));
            }

            if (QueryInfo.SupplierID)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.SupplierID);
            }

            if (QueryInfo.ItemID)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.ItemID);
            }

            if (QueryInfo.Page.Limit.has_value())
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<SuppliersItemTable> Results;

            while (Statement.executeStep())
            {
                SuppliersItemTable Entry;

                Entry.Id =
                    Statement.getColumn("Id")
                        .getUInt();

                std::memcpy(
                    (void *)Entry.SupplierID.Data(),
                    Statement.getColumn("SupplierID")
                        .getBlob(),
                    16);

                std::memcpy(
                    (void *)Entry.ItemID.Data(),
                    Statement.getColumn("ItemID")
                        .getBlob(),
                    16);

                Results.emplace_back(
                    std::move(Entry));
            }

            return Results;
        }

        std::vector<Suppliers>
        GetSuppliers(
            App &App,
            const SupplierQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * "
                   "FROM Suppliers "
                   "WHERE 1=1 ";

            if (QueryInfo.Id)
                Sql << "AND Id = ? ";

            if (QueryInfo.UniqueId)
                Sql << "AND UnqiueId = ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            if (QueryInfo.PhoneContains)
                Sql << "AND PhoneNumber LIKE ? ";

            if (QueryInfo.PanContains)
                Sql << "AND PanNumber LIKE ? ";

            if (QueryInfo.Page.Limit)
                Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.Id)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Id));
            }

            if (QueryInfo.UniqueId)
            {
                BindField(
                    Statement,
                    BindIndex++,
                    *QueryInfo.UniqueId);
            }

            if (QueryInfo.NameContains)
            {
                Statement.bind(
                    BindIndex++,
                    "%" + *QueryInfo.NameContains + "%");
            }

            if (QueryInfo.PhoneContains)
            {
                Statement.bind(
                    BindIndex++,
                    "%" + *QueryInfo.PhoneContains + "%");
            }

            if (QueryInfo.PanContains)
            {
                Statement.bind(
                    BindIndex++,
                    "%" + *QueryInfo.PanContains + "%");
            }

            if (QueryInfo.Page.Limit)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.Page.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(QueryInfo.Page.Offset));
            }

            std::vector<Suppliers> Results;

            while (Statement.executeStep())
            {
                Suppliers Entry;

                Entry.Id =
                    Statement.getColumn("Id")
                        .getUInt();

                std::memcpy(
                    (void *)Entry.UnqiueId.Data(),
                    Statement.getColumn("UnqiueId")
                        .getBlob(),
                    16);

                std::strncpy(
                    Entry.Name.data(),
                    Statement.getColumn("Name")
                        .getString()
                        .c_str(),
                    Entry.Name.size());

                std::strncpy(
                    Entry.PhoneNumber.data(),
                    Statement.getColumn("PhoneNumber")
                        .getString()
                        .c_str(),
                    Entry.PhoneNumber.size());

                std::strncpy(
                    Entry.PanNumber.data(),
                    Statement.getColumn("PanNumber")
                        .getString()
                        .c_str(),
                    Entry.PanNumber.size());

                Entry.OpeningBalance =
                    Statement.getColumn("OpeningBalance")
                        .getDouble();

                Results.emplace_back(
                    std::move(Entry));
            }

            return Results;
        }
    } // namespace ClientCpp
}
