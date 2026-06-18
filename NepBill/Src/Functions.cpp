#include <iostream>
#include <sstream>
#include <string>
#include "Tables.h"
#include "Functions.h"

namespace NepBill
{
    namespace ClientCpp
    {
        int64_t ToUnixTime(const std::tm &Time)
        {
            std::tm Copy = Time;
            return static_cast<int64_t>(std::mktime(&Copy));
        }

        void InitApp(App &App, const std::string &DbFileName)
        {
            App.Database = std::make_unique<SQLite::Database>(DbFileName, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            App.DatabaseFileName = DbFileName;
        }

        bool HasTable(const std::vector<std::string> &ActiveTableNames,
                      const std::string &Name)
        {
            return std::find(
                       ActiveTableNames.begin(),
                       ActiveTableNames.end(),
                       Name) != ActiveTableNames.end();
        }

        AskReturnCodes ShouldBusinessInfoAsk(App &App)
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
                BusinessInfo::GetName(),
                Customer::GetName(),
                Invoice::GetName(),
                InvoiceLine::GetName(),
                PaymentTicket::GetName(),
                ItemCategory::GetName(),
                Item::GetName(),
                ItemStockLedger::GetName(),
                Suppliers::GetName()};

            for (const auto &required : RequiredTables)
            {
                if (std::find(ActiveTableNames.begin(),
                              ActiveTableNames.end(),
                              required) == ActiveTableNames.end())
                {
                    return AskReturnCodes::TablesNotPresent; // missing table found
                }
            }

            // Check if we have a existing business info row?
            // if yes we return false because a business info account already exists
            // if no we allw to ask

            try
            {
                auto Info = GetBusinessInfo(App);
                if (Info == std::nullopt)
                    return AskReturnCodes::BusinessRowNotPresent;

                App.Info = Info.value();

                return AskReturnCodes::NoAsk;
            }
            catch (const SQLite::Exception &e)
            {
                std::cerr
                    << "SQLite Error: "
                    << e.what()
                    << std::endl;
                return AskReturnCodes::BusinessRowNotPresent;
            }
            catch (const std::exception &e)
            {
                std::cerr
                    << "Standard Error: "
                    << e.what()
                    << std::endl;
                return AskReturnCodes::BusinessRowNotPresent;
            }

            return AskReturnCodes::BusinessRowNotPresent;
        }

        void SetupTables(App &App)
        {
            auto Db = App.Database.get();

            try
            {
                Db->exec(NepBill::BusinessInfo::GetCreateQuerySqlite());
                Db->exec(NepBill::Customer::GetCreateQuerySqlite());
                Db->exec(NepBill::Invoice::GetCreateQuerySqlite());
                Db->exec(NepBill::InvoiceLine::GetCreateQuerySqlite());
                Db->exec(NepBill::PaymentTicket::GetCreateQuerySqlite());
                Db->exec(NepBill::ItemCategory::GetCreateQuerySqlite());
                Db->exec(NepBill::Item::GetCreateQuerySqlite());
                Db->exec(NepBill::ItemStockLedger::GetCreateQuerySqlite());
                Db->exec(NepBill::Suppliers::GetCreateQuerySqlite());
                Db->exec(NepBill::ItemCategory::GetCreateQuerySqlite());
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

        std::vector<Invoice>
        GetInvoices(App &App, const InvoiceQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM Invoice WHERE 1=1 ";

            if (QueryInfo.InvoiceId)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.SenderId)
                Sql << "AND SenderId = ? ";

            if (QueryInfo.ReceiverId)
                Sql << "AND RecieverId = ? ";

            if (QueryInfo.SenderType)
                Sql << "AND SenderType = ? ";

            if (QueryInfo.ReceiverType)
                Sql << "AND RecieverType = ? ";

            if (QueryInfo.State)
                Sql << "AND State = ? ";

            if (QueryInfo.MinTotal)
                Sql << "AND LineTotal >= ? ";

            if (QueryInfo.MaxTotal)
                Sql << "AND LineTotal <= ? ";

            if (QueryInfo.CreatedAfter)
                Sql << "AND CreatedAt >= ? ";

            if (QueryInfo.CreatedBefore)
                Sql << "AND CreatedAt <= ? ";

            if (QueryInfo.LastPaidAfter)
                Sql << "AND LastPaymentTicketDate >= ? ";

            if (QueryInfo.LastPaidBefore)
                Sql << "AND LastPaymentTicketDate <= ? ";

            Sql << "ORDER BY ";

            switch (QueryInfo.SortField)
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

            Sql << (QueryInfo.SortDescending ? " DESC " : " ASC ");

            Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.InvoiceId)
                BindField(Statement, BindIndex++, *QueryInfo.InvoiceId);

            if (QueryInfo.SenderId)
                BindField(Statement, BindIndex++, *QueryInfo.SenderId);

            if (QueryInfo.ReceiverId)
                BindField(Statement, BindIndex++, *QueryInfo.ReceiverId);

            if (QueryInfo.SenderType)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.SenderType));

            if (QueryInfo.ReceiverType)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.ReceiverType));

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

            if (QueryInfo.CreatedAfter)
                Statement.bind(
                    BindIndex++,
                    static_cast<long long>(*QueryInfo.CreatedAfter));

            if (QueryInfo.CreatedBefore)
                Statement.bind(
                    BindIndex++,
                    static_cast<long long>(*QueryInfo.CreatedBefore));

            if (QueryInfo.LastPaidAfter)
                Statement.bind(
                    BindIndex++,
                    static_cast<long long>(*QueryInfo.LastPaidAfter));

            if (QueryInfo.LastPaidBefore)
                Statement.bind(
                    BindIndex++,
                    static_cast<long long>(*QueryInfo.LastPaidBefore));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Limit));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Offset));

            std::vector<Invoice> Results;

            while (Statement.executeStep())
            {
                Invoice Inv;

                Inv.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)Inv.SenderId.Data(),
                    Statement.getColumn("SenderId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Inv.RecieverId.Data(),
                    Statement.getColumn("RecieverId").getBlob(),
                    16);

                std::memcpy(
                    (void *)Inv.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                Inv.State =
                    static_cast<InvoiceStates>(
                        Statement.getColumn("State").getUInt());

                Inv.VatPercent =
                    Statement.getColumn("VatPercent").getDouble();

                Inv.LineTotal =
                    Statement.getColumn("LineTotal").getDouble();

                Inv.SenderType =
                    static_cast<FinancialPaymentReceipientType>(
                        Statement.getColumn("SenderType").getUInt());

                Inv.RecieverType =
                    static_cast<FinancialPaymentReceipientType>(
                        Statement.getColumn("RecieverType").getUInt());

                {
                    time_t t =
                        static_cast<time_t>(
                            Statement.getColumn("CreatedAt").getInt64());

                    Inv.CreatedAt = *std::localtime(&t);
                }

                {
                    time_t t =
                        static_cast<time_t>(
                            Statement.getColumn("LastPaymentTicketDate").getInt64());

                    Inv.LastPaymentTicketDate =
                        *std::localtime(&t);
                }

                {
                    time_t t =
                        static_cast<time_t>(
                            Statement.getColumn("ClosedAt").getInt64());

                    Inv.ClosedAt =
                        *std::localtime(&t);
                }

                Results.emplace_back(std::move(Inv));
            }

            return Results;
        }

        std::optional<BusinessInfo> GetBusinessInfo(App &App)
        {
            SQLite::Statement CountQuery(
                *App.Database,
                BusinessInfo::GetCountQuery());

            if (!CountQuery.executeStep())
                return std::nullopt;

            uint32_t Count = CountQuery.getColumn(0).getInt();

            if (Count == 0)
                return std::nullopt;

            SQLite::Statement Query(
                *App.Database,
                BusinessInfo::GetByIdQuery());

            Query.bind(1, 0);

            if (!Query.executeStep())
                return std::nullopt;
            BusinessInfo Result;

            Result.Id =
                Query.getColumn("Id").getUInt();

            strncpy(
                Result.Name.data(),
                Query.getColumn("Name").getString().c_str(),
                Result.Name.size());
            strncpy(
                Result.VatNumber.data(),
                Query.getColumn("VatNumber").getString().c_str(),
                Result.VatNumber.size());
            strncpy(
                Result.Address.data(),
                Query.getColumn("Address").getString().c_str(),
                Result.Address.size());
            strncpy(
                Result.PanNumber.data(),
                Query.getColumn("PanNumber").getString().c_str(),
                Result.PanNumber.size());
            strncpy(
                Result.PhoneNumber.data(),
                Query.getColumn("PhoneNumber").getString().c_str(),
                Result.PhoneNumber.size());

            int64_t value = Query.getColumn("FiscalYearStart").getInt64();

            if (value <= 0)
            {
                // invalid timestamp → handle fallback
                Result.FiscalYearStart = {};
            }
            else
            {
                time_t createdAt = static_cast<time_t>(value);

                std::tm *t = std::localtime(&createdAt);
                if (t)
                    Result.FiscalYearStart = *t;
            }

            memcpy((void *)Result.UniqueID.Data(), Query.getColumn("UniqueID").getBlob(), 16);
            return Result;
        }

        std::vector<Item>
        GetItems(App &App, const ItemQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM Item WHERE 1=1 ";

            if (QueryInfo.ItemId)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.CategoryId)
                Sql << "AND CategoryId = ? ";

            if (QueryInfo.MinCostPrice)
                Sql << "AND CostPrice >= ? ";

            if (QueryInfo.MaxCostPrice)
                Sql << "AND CostPrice <= ? ";

            if (QueryInfo.MinSalesPrice)
                Sql << "AND SalesPrice >= ? ";

            if (QueryInfo.MaxSalesPrice)
                Sql << "AND SalesPrice <= ? ";

            if (QueryInfo.MinDiscountPercent)
                Sql << "AND DiscountPercent >= ? ";

            if (QueryInfo.MaxDiscountPercent)
                Sql << "AND DiscountPercent <= ? ";

            if (QueryInfo.MaxLowStockThreshold)
                Sql << "AND LowStockThreshold <= ? ";

            Sql << "ORDER BY ";

            switch (QueryInfo.SortField)
            {
            case ItemSortField::Name:
                Sql << "Name";
                break;

            case ItemSortField::CostPrice:
                Sql << "CostPrice";
                break;

            case ItemSortField::SalesPrice:
                Sql << "SalesPrice";
                break;

            case ItemSortField::DiscountPercent:
                Sql << "DiscountPercent";
                break;

            case ItemSortField::LowStockThreshold:
                Sql << "LowStockThreshold";
                break;
            }

            Sql << (QueryInfo.SortDescending ? " DESC " : " ASC ");

            Sql << "LIMIT ? OFFSET ?";

            SQLite::Statement Statement(
                *App.Database,
                Sql.str());

            int BindIndex = 1;

            if (QueryInfo.ItemId)
                BindField(Statement, BindIndex++, *QueryInfo.ItemId);

            if (QueryInfo.CategoryId)
                BindField(Statement, BindIndex++, *QueryInfo.CategoryId);

            if (QueryInfo.MinCostPrice)
                Statement.bind(BindIndex++, *QueryInfo.MinCostPrice);

            if (QueryInfo.MaxCostPrice)
                Statement.bind(BindIndex++, *QueryInfo.MaxCostPrice);

            if (QueryInfo.MinSalesPrice)
                Statement.bind(BindIndex++, *QueryInfo.MinSalesPrice);

            if (QueryInfo.MaxSalesPrice)
                Statement.bind(BindIndex++, *QueryInfo.MaxSalesPrice);

            if (QueryInfo.MinDiscountPercent)
                Statement.bind(BindIndex++, *QueryInfo.MinDiscountPercent);

            if (QueryInfo.MaxDiscountPercent)
                Statement.bind(BindIndex++, *QueryInfo.MaxDiscountPercent);

            if (QueryInfo.MaxLowStockThreshold)
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(*QueryInfo.MaxLowStockThreshold));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Limit));

            Statement.bind(
                BindIndex++,
                static_cast<int>(QueryInfo.Offset));

            std::vector<Item> Results;

            while (Statement.executeStep())
            {
                Item ItemObj;

                ItemObj.Id =
                    Statement.getColumn("Id").getUInt();

                std::memcpy(
                    (void *)ItemObj.CategoryId.Data(),
                    Statement.getColumn("CategoryId").getBlob(),
                    16);

                std::memcpy(
                    (void *)ItemObj.UniqueID.Data(),
                    Statement.getColumn("UniqueID").getBlob(),
                    16);

                std::strncpy(
                    ItemObj.Name.data(),
                    Statement.getColumn("Name")
                        .getString()
                        .c_str(),
                    ItemObj.Name.size());

                ItemObj.LowStockThresold =
                    Statement.getColumn("LowStockThreshold")
                        .getUInt();

                ItemObj.CostPrice =
                    Statement.getColumn("CostPrice")
                        .getDouble();

                ItemObj.SalesPrice =
                    Statement.getColumn("SalesPrice")
                        .getDouble();

                ItemObj.DiscountPercent =
                    Statement.getColumn("DiscountPercent")
                        .getDouble();

                std::strncpy(
                    ItemObj.Description.data(),
                    Statement.getColumn("Description")
                        .getString()
                        .c_str(),
                    ItemObj.Description.size());

                Results.emplace_back(
                    std::move(ItemObj));
            }

            return Results;
        }

        std::vector<ItemCategory>
        GetItemCategories(
            App &App,
            const ItemCategoryQuery &QueryInfo)
        {
            std::ostringstream Sql;

            Sql << "SELECT * FROM ItemCategory WHERE 1=1 ";

            if (QueryInfo.UniqueID)
                Sql << "AND UniqueID = ? ";

            if (QueryInfo.NameContains)
                Sql << "AND Name LIKE ? ";

            Sql << "ORDER BY ";

            switch (QueryInfo.SortField)
            {
            case ItemCategorySortField::Name:
                Sql << "Name";
                break;

            case ItemCategorySortField::Id:
                Sql << "Id";
                break;
            }

            Sql << (QueryInfo.SortDescending
                        ? " DESC "
                        : " ASC ");

            if (QueryInfo.Limit != UINT32_MAX)
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
            {
                Statement.bind(
                    BindIndex++,
                    "%" +
                        *QueryInfo.NameContains +
                        "%");
            }

            if (QueryInfo.Limit != UINT32_MAX)
            {
                Statement.bind(
                    BindIndex++,
                    static_cast<int>(
                        QueryInfo.Limit));

                Statement.bind(
                    BindIndex++,
                    static_cast<int>(
                        QueryInfo.Offset));
            }

            std::vector<ItemCategory> Results;

            while (Statement.executeStep())
            {
                ItemCategory Category;

                Category.Id =
                    Statement.getColumn("Id")
                        .getUInt();

                std::memcpy(
                    (void *)Category.UniqueID.Data(),
                    Statement.getColumn("UniqueID")
                        .getBlob(),
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
    } // namespace ClientCpp

}