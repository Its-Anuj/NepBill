#ifndef NEPBILL_H
#define NEPBILL_H

#include <optional>
#include "Tables.h"
#include "Query.h"

namespace NepBill
{
    struct App
    {
        pqxx::connection Database;

        App(const std::string &ConnString)
            : Database(ConnString)
        {
        }
    };

    inline int64_t ToUnixTime(const std::tm &Time)
    {
        std::tm Copy = Time;
        return static_cast<int64_t>(std::mktime(&Copy));
    }

    // Function to get time exactly one week ago
    inline std::tm getTimeOneWeekAgo()
    {
        // 1. Get current time as seconds since epoch
        std::time_t nowTime = std::time(nullptr);

        // 2. Subtract exactly 7 days (7 * 24 * 60 * 60 seconds)
        const int64_t oneWeekSeconds = 7 * 24 * 60 * 60;
        std::time_t weekAgoTime = nowTime - oneWeekSeconds;

        // 3. Convert back to struct tm
        // Note: localtime is not thread-safe. Use localtime_r (POSIX) or localtime_s (Windows) in production.
        std::tm weekAgoTm = *std::localtime(&weekAgoTime);

        return weekAgoTm;
    }

    inline std::tm getCurrentTime()
    {
        std::time_t Now = std::time(nullptr);
        return *std::localtime(&Now);
    }

    inline std::string getPostgresTimestamp(std::time_t now)
    {
        // Convert to local time structure (or use gmtime for UTC)
        std::tm *now_tm = std::localtime(&now);

        // Format to YYYY-MM-DD HH:MM:SS
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);

        return std::string(buffer);
    }

    inline std::string getPostgresTimestamp(const struct tm &now)
    {
        auto now_tm = &now;
        // Format to YYYY-MM-DD HH:MM:SS
        char buffer[20];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);

        return std::string(buffer);
    }

    inline void Insert(pqxx::connection &Db, const ContactFormStateMetaInfo &Info)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.FormID.ToString(),
                Info.StateMessage.data()});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const ContactFormInfo &Info)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.UniqueId.ToString(),
                Info.PhoneNumber.data(),
                Info.Email.data(),
                Info.Message.data(),
                static_cast<int>(Info.Type),
                getPostgresTimestamp(ToUnixTime(getCurrentTime())),
                static_cast<int>(Info.State)});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const RegisterAccountContactFormInfo &Info)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.ContactID.ToString(),
                Info.Name.data(),
                Info.Ownername.data(),
                Info.PrimaryPhoneNumber.data(),
                Info.VatNumber.data(),
                Info.PanNumber.data(),
                Info.Country.data(),
                Info.Address.data(),
                Info.Password.data(),
                Info.VatCertificatePdf.ToString(),
                Info.PanPdf.ToString(),
                Info.BusinessFrontImage.ToString()});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const PdfDocument &Info)
    {
        const auto *byte_data = reinterpret_cast<const std::byte *>(Info.Data.data());

        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.UniqueId.ToString(),
                Info.Name.data(),
                pqxx::bytes{byte_data, byte_data + Info.Data.size()}});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const Image &Info)
    {
        const auto *byte_data = reinterpret_cast<const std::byte *>(Info.Data.data());
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.UniqueId.ToString(),
                Info.Name.data(),
                pqxx::bytes{byte_data, byte_data + Info.Data.size()}});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const EncryptedSecret &Info)
    {
        const auto *salt_bytes = reinterpret_cast<const std::byte *>(Info.KeySalt.data());
        const auto *nonce_bytes = reinterpret_cast<const std::byte *>(Info.EncryptionNonce.data());
        const auto *ciphertext_bytes = reinterpret_cast<const std::byte *>(Info.Ciphertext.data());
        const std::size_t valid_ciphertext_len = std::min(Info.ActualCiphertextSize, Info.Ciphertext.size());

        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.AccountID.ToString(),
                pqxx::bytes{salt_bytes, salt_bytes + Info.KeySalt.size()},
                pqxx::bytes{nonce_bytes, nonce_bytes + Info.EncryptionNonce.size()},
                pqxx::bytes{ciphertext_bytes, ciphertext_bytes + valid_ciphertext_len},
                Info.ActualCiphertextSize});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const Business &Info)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.UniqueId.ToString(),
                Info.RegisterAccountFormId.ToString(),
                getPostgresTimestamp(ToUnixTime(getCurrentTime()))});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const Account &Info)
    {
        std::optional<std::string> parent_id_str = std::nullopt;
        if (Info.ParentID != UUID::InvalidUUID())
        {
            parent_id_str = Info.ParentID.ToString();
        }
        pqxx::work Tx(Db);
        Tx.exec(
            Info.GetInsertQuery(),
            pqxx::params{
                Info.UniqueId.ToString(),
                Info.BusinessID.ToString(),
                Info.ContactFormID.ToString(),
                parent_id_str,
                Info.PasswordHash.data(),
                static_cast<int>(Info.Role),
                Info.IsActive});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const ItemStockLedger &Ledger)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Ledger.GetInsertQuery(),
            pqxx::params{
                Ledger.BusinessID.ToString(),
                Ledger.PurchaseOrderID.ToString(),
                Ledger.ItemId.ToString(),
                Ledger.StockDelta,
                Ledger.AccountID.ToString(),
                static_cast<int>(Ledger.Reason),
                getPostgresTimestamp(ToUnixTime(Ledger.CreatedAt))});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const PurchaseOrder &Order)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Order.GetInsertQuery(),
            pqxx::params{
                Order.UniqueID.ToString(),
                Order.BusinessID.ToString(),
                Order.SupplierID.ToString(),
                Order.ItemInvoiceId.ToString(),
                static_cast<int>(Order.State),
                getPostgresTimestamp(ToUnixTime(Order.CreatedAt))});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const PurchaseOrderLine &LineObj)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            LineObj.GetInsertQuery(),
            pqxx::params{
                LineObj.UniqueID.ToString(),
                LineObj.PurchaseOrderID.ToString(),
                LineObj.ItemID.ToString(),
                LineObj.OrderedQuantity,
                LineObj.ReceivedQuantity,
                static_cast<uint32_t>(LineObj.PurchaseUnit),
                LineObj.PurchaseToStockConversion,
                static_cast<uint32_t>(LineObj.StockUnit),
                LineObj.UnitPrice,
                LineObj.DiscountPercent,
                LineObj.Remarks});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const ItemInvoice &Invoice)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Invoice.GetInsertQuery(),
            pqxx::params{
                Invoice.SenderId.ToString(),
                Invoice.RecieverId.ToString(),
                Invoice.UniqueID.ToString(),
                static_cast<int>(Invoice.State),
                Invoice.VatPercent,
                Invoice.LineTotal,
                static_cast<int>(Invoice.SenderType.Type),
                static_cast<uint32_t>(Invoice.SenderType.ServiceType),
                static_cast<int>(Invoice.RecieverType.Type),
                static_cast<uint32_t>(Invoice.RecieverType.ServiceType),
                getPostgresTimestamp(ToUnixTime(Invoice.CreatedAt)),
                getPostgresTimestamp(ToUnixTime(Invoice.LastPaymentTicketDate)),
                getPostgresTimestamp(ToUnixTime(Invoice.ClosedAt))});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const ItemInvoiceLine &Line)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Line.GetInsertQuery(),
            pqxx::params{
                Line.InvoiceId.ToString(),
                Line.ItemId.ToString(),
                Line.UnqiueId.ToString(),
                Line.OrderedQuantity,
                Line.DeliveredQuantity,
                Line.ItemWeight,
                Line.UnitPrice,
                Line.UnitDiscountPercet,
                Line.LineTotal});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const Suppliers &SupplierObj)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            SupplierObj.GetInsertQuery(),
            pqxx::params{
                SupplierObj.BusinessID.ToString(),
                SupplierObj.UnqiueId.ToString(),
                SupplierObj.Name,
                SupplierObj.PhoneNumber,
                SupplierObj.PanNumber,
                SupplierObj.OpeningBalance});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const ItemCategory &Category)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            Category.GetInsertQuery(),
            pqxx::params{
                Category.BusinessID.ToString(),
                Category.UniqueID.ToString(),
                Category.Name});
        Tx.commit();
    }

    inline void Insert(pqxx::connection &Db, const Item &ItemObj)
    {
        pqxx::work Tx(Db);
        Tx.exec(
            ItemObj.GetInsertQuery(),
            pqxx::params{
                ItemObj.BusinessID.ToString(),
                ItemObj.CategoryId.ToString(),
                ItemObj.UniqueID.ToString(),
                ItemObj.Name,
                ItemObj.LowStockThresold,
                ItemObj.TracksStock,
                ItemObj.Description,
                static_cast<uint32_t>(ItemObj.StockUnit),
                ItemObj.DefaultPurchaseConversion,
                static_cast<uint32_t>(ItemObj.DefaultPurchaseUnit)});
        Tx.commit();
    }

    std::vector<ContactFormInfo> GetContactForms(
        pqxx::connection &Connection,
        const ContactFormQuery &Query);

    std::vector<Image> GetImages(
        pqxx::connection &Connection,
        const ImageQuery &Query);

    std::vector<RegisterAccountContactFormInfo> GetRegisterAccountContactForms(
        pqxx::connection &Connection,
        const RegisterAccountContactFormQuery &Query);

    std::vector<PdfDocument> GetPdfDocuments(
        pqxx::connection &Connection,
        const PdfDocumentQuery &Query);

    std::vector<ContactFormStateMetaInfo> GetContactFormStateMetaInfo(
        pqxx::connection &Connection,
        const ContactFormStateMetaInfoQuery &Query);

    std::vector<Business> GetBusinesses(
        pqxx::connection &Connection,
        const BusinessQuery &Query);

    std::vector<Account> GetAccounts(
        pqxx::connection &Connection,
        const AccountQuery &Query);

    std::vector<EncryptedSecret> GetEncryptedSecrets(
        pqxx::connection &Connection,
        const EncryptedSecretQuery &Query);

    bool UpdateContactForm(
        pqxx::connection &Connection,
        const UpdateContactFormQuery &UpdateQuery);

    bool UpdateContactForm(
        pqxx::connection &Connection,
        const ContactFormInfo &Info);

    std::vector<ItemStockLedger> GetItemStockLedgers(
        pqxx::connection &Connection,
        const ItemStockLedgerQuery &Query);

    std::vector<PurchaseOrder> GetPurchaseOrders(
        pqxx::connection &Connection,
        const PurchaseOrderQuery &Query);

    std::vector<PurchaseOrderLine> GetPurchaseOrderLines(
        pqxx::connection &Connection,
        const PurchaseOrderLineQuery &Query);

    std::vector<ItemInvoiceLine> GetItemInvoiceLines(
        pqxx::connection &Connection,
        const ItemInvoiceLineQuery &Query);

    std::vector<ItemInvoice> GetItemInvoices(
        pqxx::connection &Connection,
        const ItemInvoiceQuery &Query);

    std::vector<Suppliers> GetSuppliers(
        pqxx::connection &Connection,
        const SuppliersQuery &Query);

    std::vector<Item> GetItems(
        pqxx::connection &Connection,
        const ItemQuery &Query);

    std::vector<ItemCategory> GetItemCategories(
        pqxx::connection &Connection,
        const ItemCategoryQuery &Query);
} // namespace NepBill

#endif