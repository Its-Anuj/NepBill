#include <PCH.h>
#include "NepBill.h"

namespace NepBill
{
    void Update(
        pqxx::connection &Db,
        uint32_t Id,
        const ContactFormInfo &ContactForm,
        bool Commit)
    {
        pqxx::work Tx(Db);

        Tx.exec_params(
            R"(
        UPDATE ContactFormInfo
        SET
            UniqueId = $1,
            PhoneNumber = $2,
            Message = $3,
            Type = $4,
            CreatedAt = $5,
            State = $6
        WHERE Id = $7
        )",
            ContactForm.UniqueId.ToString(),
            ContactForm.PhoneNumber.data(),
            ContactForm.Message.data(),
            static_cast<int>(ContactForm.Type),
            getPostgresTimestamp(ContactForm.CreatedAt),
            ContactForm.State,
            Id);

        if (Commit)
            Tx.commit();
    }

    std::vector<ContactFormInfo>
    GetContactForms(
        pqxx::connection &Connection,
        const ContactFormQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id,"
            "UniqueId,"
            "PhoneNumber,"
            "Email,"
            "Message,"
            "Type,"
            "EXTRACT(EPOCH FROM CreatedAt)::BIGINT AS CreatedAt," // Converted to int64_t epoch seconds
            "State "
            "FROM ContactFormInfo "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.UniqueId)
        {
            Sql += " AND UniqueId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueId->ToString());
        }

        if (Query.PhoneNumber)
        {
            Sql += " AND PhoneNumber = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.PhoneNumber);
        }

        if (Query.Email)
        {
            Sql += " AND Email = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Email);
        }

        if (Query.Type)
        {
            Sql += " AND Type = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string((int)*Query.Type));
        }

        if (Query.State)
        {
            Sql += " AND State = $" + std::to_string(Params.size() + 1);
            // Cast to int before converting to string:
            Params.push_back(std::to_string(static_cast<int>(*Query.State)));
        }

        if (Query.CreatedAfter)
        {
            Sql += " AND CreatedAt >= to_timestamp($" + std::to_string(Params.size() + 1) + ")";
            Params.push_back(std::to_string(*Query.CreatedAfter));
        }

        if (Query.CreatedBefore)
        {
            Sql += " AND CreatedAt <= to_timestamp($" + std::to_string(Params.size() + 1) + ")";
            Params.push_back(std::to_string(*Query.CreatedBefore));
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ContactSortField::CreatedAt:
            Sql += "CreatedAt";
            break;

        case ContactSortField::PhoneNumber:
            Sql += "PhoneNumber";
            break;
        }

        Sql += Query.SortDescending ? " DESC" : " ASC";

        if (Query.Pagination.Limit)
        {
            Sql += " LIMIT " + std::to_string(*Query.Pagination.Limit);
        }

        Sql += " OFFSET " + std::to_string(Query.Pagination.Offset);

        pqxx::params P;

        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);

        std::vector<ContactFormInfo> Contacts;

        for (auto Row : Result)
        {
            ContactFormInfo Contact;

            Contact.Id = Row["Id"].as<uint32_t>();

            Contact.UniqueId =
                UUID::FromString(Row["UniqueId"].c_str());

            std::strncpy(
                Contact.PhoneNumber.data(),
                Row["PhoneNumber"].c_str(),
                Contact.PhoneNumber.size());

            std::strncpy(
                Contact.Email.data(),
                Row["Email"].c_str(),
                Contact.Email.size());

            std::strncpy(
                Contact.Message.data(),
                Row["Message"].c_str(),
                Contact.Message.size());

            Contact.Type =
                (ContactSubjectType)Row["Type"].as<int>();

            Contact.State =
                (ContactStates)Row["State"].as<int>();

            std::time_t T =
                Row["CreatedAt"].as<std::time_t>();

            Contact.CreatedAt =
                *std::localtime(&T);

            Contacts.push_back(Contact);
        }

        Tx.commit();

        return Contacts;
    }

    std::vector<RegisterAccountContactFormInfo>
    GetRegisterAccountContactForms(
        pqxx::connection &Connection,
        const RegisterAccountContactFormQuery &Query)
    {
        pqxx::work Tx(Connection);

        std::string Sql =
            "SELECT "
            "Id,"
            "ContactID,"
            "Name,"
            "PrimaryPhoneNumber,"
            "VatNumber,"
            "PanNumber,"
            "Country,"
            "Address "
            "FROM RegisterAccountContactFormInfo "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.ContactID)
        {
            Sql += " AND ContactID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ContactID->ToString());
        }

        if (Query.Name)
        {
            Sql += " AND Name = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Name);
        }

        if (Query.PrimaryPhoneNumber)
        {
            Sql += " AND PrimaryPhoneNumber = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.PrimaryPhoneNumber);
        }

        if (Query.VatNumber)
        {
            Sql += " AND VatNumber = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.VatNumber);
        }

        if (Query.PanNumber)
        {
            Sql += " AND PanNumber = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.PanNumber);
        }

        if (Query.Country)
        {
            Sql += " AND Country = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Country);
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case RegisterAccountSortField::Name:
            Sql += "Name";
            break;

        case RegisterAccountSortField::PrimaryPhoneNumber:
            Sql += "PrimaryPhoneNumber";
            break;
        }

        Sql += Query.SortDescending ? " DESC" : " ASC";

        if (Query.Pagination.Limit)
        {
            Sql += " LIMIT " +
                   std::to_string(*Query.Pagination.Limit);
        }

        Sql += " OFFSET " +
               std::to_string(Query.Pagination.Offset);

        pqxx::params P;

        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);

        std::vector<RegisterAccountContactFormInfo> Forms;

        for (auto Row : Result)
        {
            RegisterAccountContactFormInfo Form;

            Form.Id =
                Row["Id"].as<uint32_t>();

            Form.ContactID =
                UUID::FromString(Row["ContactID"].c_str());

            std::strncpy(
                Form.Name.data(),
                Row["Name"].c_str(),
                Form.Name.size());

            std::strncpy(
                Form.PrimaryPhoneNumber.data(),
                Row["PrimaryPhoneNumber"].c_str(),
                Form.PrimaryPhoneNumber.size());

            std::strncpy(
                Form.VatNumber.data(),
                Row["VatNumber"].c_str(),
                Form.VatNumber.size());

            std::strncpy(
                Form.PanNumber.data(),
                Row["PanNumber"].c_str(),
                Form.PanNumber.size());

            std::strncpy(
                Form.Country.data(),
                Row["Country"].c_str(),
                Form.Country.size());

            std::strncpy(
                Form.Address.data(),
                Row["Address"].c_str(),
                Form.Address.size());

            Forms.push_back(Form);
        }

        Tx.commit();

        return Forms;
    }

    std::vector<AccountRoleInfo> GetAccountRoles(
        pqxx::connection &Connection,
        const AccountRoleQuery &Query)
    {
        pqxx::work Tx(Connection);

        std::string Sql =
            "SELECT "
            "Id,"
            "AccountID,"
            "Role "
            "FROM AccountRoleInfo "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.AccountID)
        {
            Sql += " AND AccountID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.AccountID->ToString());
        }

        if (Query.Role)
        {
            Sql += " AND Role = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<int32_t>(*Query.Role)));
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case AccountRoleSortField::Id:
            Sql += "Id";
            break;

        case AccountRoleSortField::AccountID:
            Sql += "AccountID";
            break;

        case AccountRoleSortField::Role:
            Sql += "Role";
            break;
        }

        Sql += Query.SortDescending ? " DESC" : " ASC";

        if (Query.Pagination.Limit)
        {
            Sql += " LIMIT " +
                   std::to_string(*Query.Pagination.Limit);
        }

        Sql += " OFFSET " +
               std::to_string(Query.Pagination.Offset);

        pqxx::params P;

        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);

        std::vector<AccountRoleInfo> Roles;

        for (auto Row : Result)
        {
            AccountRoleInfo Info;

            Info.Id =
                Row["Id"].as<uint32_t>();

            Info.AccountID =
                UUID::FromString(Row["AccountID"].c_str());

            Info.Role =
                static_cast<AccountRoles>(Row["Role"].as<int32_t>());

            Roles.push_back(Info);
        }

        Tx.commit();

        return Roles;
    }

    std::vector<ContactFormStateMetaInfo> GetContactFormStateMetaInfos(
        pqxx::connection &Connection,
        const ContactFormStateMetaQuery &Query)
    {
        pqxx::work Tx(Connection);

        std::string Sql =
            "SELECT "
            "Id,"
            "FormID,"
            "StateMessage "
            "FROM ContactFormStateMetaInfo "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.FormID)
        {
            Sql += " AND FormID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.FormID->ToString());
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ContactFormStateMetaSortField::Id:
            Sql += "Id";
            break;

        case ContactFormStateMetaSortField::FormID:
            Sql += "FormID";
            break;
        }

        Sql += Query.SortDescending ? " DESC" : " ASC";

        if (Query.Pagination.Limit)
        {
            Sql += " LIMIT " +
                   std::to_string(*Query.Pagination.Limit);
        }

        Sql += " OFFSET " +
               std::to_string(Query.Pagination.Offset);

        pqxx::params P;

        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);

        std::vector<ContactFormStateMetaInfo> MetaInfos;

        for (auto Row : Result)
        {
            ContactFormStateMetaInfo Info;

            Info.Id =
                Row["Id"].as<uint32_t>();

            Info.FormID =
                UUID::FromString(Row["FormID"].c_str());

            std::strncpy(
                Info.StateMessage.data(),
                Row["StateMessage"].c_str(),
                Info.StateMessage.size());

            MetaInfos.push_back(Info);
        }

        Tx.commit();

        return MetaInfos;
    }
} // namespace NepBill
