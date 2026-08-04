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

    inline std::tm GetCurrentTime()
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

    inline void Insert(pqxx::connection &Db,
                       const ContactFormInfo &ContactForm, bool Commit = false)
    {
        pqxx::work Tx(Db);
        auto CreatedAt = GetCurrentTime();

        Tx.exec_params(
            ContactForm.GetInsertQuery(),
            ContactForm.UniqueId.ToString(),
            ContactForm.PhoneNumber.data(),
            ContactForm.Email.data(),
            ContactForm.Message.data(),
            static_cast<int>(ContactForm.Type),
            getPostgresTimestamp(CreatedAt),
            static_cast<int>(ContactForm.State));

        if (Commit)
            Tx.commit();
    }

    inline void Insert(
        pqxx::connection &Db,
        const RegisterAccountContactFormInfo &Info,
        bool Commit = false)
    {
        pqxx::work Tx(Db);

        Tx.exec_params(
            Info.GetInsertQuery(),
            Info.ContactID.ToString(),
            Info.Name.data(),
            Info.PrimaryPhoneNumber.data(),
            Info.VatNumber.data(),
            Info.PanNumber.data(),
            Info.Country.data(),
            Info.Address.data());

        if (Commit)
            Tx.commit();
    }

        inline void Insert(
        pqxx::connection &Db,
        const AccountRoleInfo &Info,
        bool Commit = false)
    {
        pqxx::work Tx(Db);

        Tx.exec_params(
            Info.GetInsertQuery(),
            Info.AccountID.ToString(),
            static_cast<int32_t>(Info.Role));

        if (Commit)
            Tx.commit();
    }

inline void Insert(
    pqxx::connection &Db,
    const ContactFormStateMetaInfo &Info,
    bool Commit = false)
{
    pqxx::work Tx(Db);

    Tx.exec_params(
        Info.GetInsertQuery(),
        Info.FormID.ToString(),
        Info.StateMessage.data());

    if (Commit)
        Tx.commit();
}

    std::vector<ContactFormInfo> GetContactForms(pqxx::connection &Connection, const ContactFormQuery &Query);
    void Update(pqxx::connection &Db, uint32_t Id, const ContactFormInfo &ContactForm, bool Commit = true);

    std::vector<RegisterAccountContactFormInfo> GetRegisterAccountContactForms(pqxx::connection &Connection, const RegisterAccountContactFormQuery &Query);
    std::vector<AccountRoleInfo> GetAccountRoles(pqxx::connection &Connection,const AccountRoleQuery &Query);
    std::vector<ContactFormStateMetaInfo> GetContactFormStateMetaInfos(pqxx::connection &Connection,const ContactFormStateMetaQuery &Query);

} // namespace NepBill

#endif