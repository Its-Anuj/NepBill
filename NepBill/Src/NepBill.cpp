#include <PCH.h>
#include "NepBill.h"

namespace NepBill
{
    std::vector<RegisterAccountContactFormInfo> GetRegisterAccountContactForms(
        pqxx::connection &Connection,
        const RegisterAccountContactFormQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "ContactID, "
            "Name, "
            "Ownername, "
            "PrimaryPhoneNumber, "
            "VatNumber, "
            "PanNumber, "
            "Country, "
            "Address, "
            "Password, "
            "VatCertificatePdf, "
            "PanPdf, "
            "BusinessFrontImage "
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

        if (Query.Ownername)
        {
            Sql += " AND Ownername = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Ownername);
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
        case RegisterAccountContactSortField::Id:
            Sql += "Id";
            break;

        case RegisterAccountContactSortField::Name:
            Sql += "Name";
            break;

        case RegisterAccountContactSortField::Ownername:
            Sql += "Ownername";
            break;

        case RegisterAccountContactSortField::Country:
            Sql += "Country";
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

        std::vector<RegisterAccountContactFormInfo> Forms;

        for (auto Row : Result)
        {
            RegisterAccountContactFormInfo Form;

            Form.Id = Row["Id"].as<uint32_t>();
            Form.ContactID = UUID::FromString(Row["ContactID"].c_str());

            std::strncpy(Form.Name.data(), Row["Name"].c_str(), Form.Name.size());
            std::strncpy(Form.Ownername.data(), Row["Ownername"].c_str(), Form.Ownername.size());
            std::strncpy(Form.PrimaryPhoneNumber.data(), Row["PrimaryPhoneNumber"].c_str(), Form.PrimaryPhoneNumber.size());
            std::strncpy(Form.VatNumber.data(), Row["VatNumber"].c_str(), Form.VatNumber.size());
            std::strncpy(Form.PanNumber.data(), Row["PanNumber"].c_str(), Form.PanNumber.size());
            std::strncpy(Form.Country.data(), Row["Country"].c_str(), Form.Country.size());
            std::strncpy(Form.Address.data(), Row["Address"].c_str(), Form.Address.size());
            std::strncpy(Form.Password.data(), Row["Password"].c_str(), Form.Password.size());

            Form.VatCertificatePdf = UUID::FromString(Row["VatCertificatePdf"].c_str());
            Form.PanPdf = UUID::FromString(Row["PanPdf"].c_str());
            Form.BusinessFrontImage = UUID::FromString(Row["BusinessFrontImage"].c_str());

            Forms.push_back(Form);
        }

        Tx.commit();
        return Forms;
    }

    std::vector<Business> GetBusinesses(
        pqxx::connection &Connection,
        const BusinessQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueId, "
            "RegisterAccountFormId, "
            "CreatedAt "
            "FROM Business "
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

        if (Query.RegisterAccountFormId)
        {
            Sql += " AND RegisterAccountFormId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.RegisterAccountFormId->ToString());
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case BusinessSortField::Id:
            Sql += "Id";
            break;

        case BusinessSortField::UniqueId:
            Sql += "UniqueId";
            break;

        case BusinessSortField::RegisterAccountFormId:
            Sql += "RegisterAccountFormId";
            break;

        case BusinessSortField::CreatedAt:
            Sql += "CreatedAt";
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

        std::vector<Business> Businesses;

        for (auto Row : Result)
        {
            Business Item;

            Item.Id = Row["Id"].as<uint32_t>();
            Item.UniqueId = UUID::FromString(Row["UniqueId"].c_str());
            Item.RegisterAccountFormId = UUID::FromString(Row["RegisterAccountFormId"].c_str());

            std::time_t T = Row["CreatedAt"].as<std::time_t>();
            Item.CreatedAt = *std::localtime(&T);

            Businesses.push_back(Item);
        }

        Tx.commit();
        return Businesses;
    }

    std::vector<EncryptedSecret> GetEncryptedSecrets(
        pqxx::connection &Connection,
        const EncryptedSecretQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "AccountID, "
            "KeySalt, "
            "EncryptionNonce, "
            "Ciphertext, "
            "ActualCiphertextSize "
            "FROM EncryptedSecret "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.AccountID)
        {
            Sql += " AND AccountID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.AccountID->ToString());
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case EncryptedSecretSortField::AccountID:
            Sql += "AccountID";
            break;

        case EncryptedSecretSortField::ActualCiphertextSize:
            Sql += "ActualCiphertextSize";
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

        std::vector<EncryptedSecret> Secrets;

        for (auto Row : Result)
        {
            EncryptedSecret Item;

            Item.AccountID = UUID::FromString(Row["AccountID"].c_str());

            auto KeySaltBytes = Row["KeySalt"].as<pqxx::bytes>();
            std::memcpy(Item.KeySalt.data(), KeySaltBytes.data(), std::min(Item.KeySalt.size(), KeySaltBytes.size()));

            auto NonceBytes = Row["EncryptionNonce"].as<pqxx::bytes>();
            std::memcpy(Item.EncryptionNonce.data(), NonceBytes.data(), std::min(Item.EncryptionNonce.size(), NonceBytes.size()));

            auto CiphertextBytes = Row["Ciphertext"].as<pqxx::bytes>();
            std::memcpy(Item.Ciphertext.data(), CiphertextBytes.data(), std::min(Item.Ciphertext.size(), CiphertextBytes.size()));

            Item.ActualCiphertextSize = Row["ActualCiphertextSize"].as<std::size_t>();

            Secrets.push_back(Item);
        }

        Tx.commit();
        return Secrets;
    }

    std::vector<Account> GetAccounts(
        pqxx::connection &Connection,
        const AccountQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueId, "
            "BusinessID, "
            "ContactFormID, "
            "ParentID, "
            "PasswordHash, "
            "Role, "
            "IsActive "
            "FROM Account "
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

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.ContactFormID)
        {
            Sql += " AND ContactFormID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ContactFormID->ToString());
        }

        if (Query.ParentID)
        {
            Sql += " AND ParentID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ParentID->ToString());
        }

        if (Query.Role)
        {
            Sql += " AND Role = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<int>(*Query.Role)));
        }

        if (Query.IsActive)
        {
            Sql += " AND IsActive = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.IsActive ? "TRUE" : "FALSE");
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case AccountSortField::Id:
            Sql += "Id";
            break;

        case AccountSortField::UniqueId:
            Sql += "UniqueId";
            break;

        case AccountSortField::BusinessID:
            Sql += "BusinessID";
            break;

        case AccountSortField::ContactFormID:
            Sql += "ContactFormID";
            break;

        case AccountSortField::Role:
            Sql += "Role";
            break;

        case AccountSortField::IsActive:
            Sql += "IsActive";
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

        std::vector<Account> Accounts;

        for (auto Row : Result)
        {
            Account Acc;

            Acc.Id = Row["Id"].as<uint32_t>();
            Acc.UniqueId = UUID::FromString(Row["UniqueId"].c_str());
            Acc.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            Acc.ContactFormID = UUID::FromString(Row["ContactFormID"].c_str());

            if (!Row["ParentID"].is_null())
            {
                Acc.ParentID = UUID::FromString(Row["ParentID"].c_str());
            }

            std::strncpy(Acc.PasswordHash.data(), Row["PasswordHash"].c_str(), Acc.PasswordHash.size());
            Acc.Role = static_cast<AccountRoles>(Row["Role"].as<int>());
            Acc.IsActive = Row["IsActive"].as<bool>();

            Accounts.push_back(Acc);
        }

        Tx.commit();
        return Accounts;
    }

    std::vector<PdfDocument> GetPdfDocuments(
        pqxx::connection &Connection,
        const PdfDocumentQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueId, "
            "Name, "
            "Data "
            "FROM PdfDocument "
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

        if (Query.Name)
        {
            Sql += " AND Name = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Name);
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case PdfDocumentSortField::Id:
            Sql += "Id";
            break;

        case PdfDocumentSortField::Name:
            Sql += "Name";
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

        std::vector<PdfDocument> Documents;

        for (auto Row : Result)
        {
            PdfDocument Doc;

            Doc.Id = Row["Id"].as<uint32_t>();
            Doc.UniqueId = UUID::FromString(Row["UniqueId"].c_str());

            std::strncpy(Doc.Name.data(), Row["Name"].c_str(), Doc.Name.size());

            auto ByteData = Row["Data"].as<pqxx::bytes>();
            Doc.Data.assign(
                reinterpret_cast<const uint8_t *>(ByteData.data()),
                reinterpret_cast<const uint8_t *>(ByteData.data()) + ByteData.size());

            Documents.push_back(Doc);
        }

        Tx.commit();
        return Documents;
    }

    std::vector<Image> GetImages(
        pqxx::connection &Connection,
        const ImageQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueId, "
            "Name, "
            "Data "
            "FROM Image "
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

        if (Query.Name)
        {
            Sql += " AND Name = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Name);
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ImageSortField::Id:
            Sql += "Id";
            break;

        case ImageSortField::Name:
            Sql += "Name";
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

        std::vector<Image> Images;

        for (auto Row : Result)
        {
            Image Img;

            Img.Id = Row["Id"].as<uint32_t>();
            Img.UniqueId = UUID::FromString(Row["UniqueId"].c_str());

            std::strncpy(Img.Name.data(), Row["Name"].c_str(), Img.Name.size());

            auto ByteData = Row["Data"].as<pqxx::bytes>();
            Img.Data.assign(
                reinterpret_cast<const uint8_t *>(ByteData.data()),
                reinterpret_cast<const uint8_t *>(ByteData.data()) + ByteData.size());

            Images.push_back(Img);
        }

        Tx.commit();
        return Images;
    }

    std::vector<ContactFormStateMetaInfo> GetContactFormStateMetaInfo(
        pqxx::connection &Connection,
        const ContactFormStateMetaInfoQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "FormID, "
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
        case ContactStateMetaSortField::Id:
            Sql += "Id";
            break;
        case ContactStateMetaSortField::FormID:
            Sql += "FormID";
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

        std::vector<ContactFormStateMetaInfo> List;

        for (auto Row : Result)
        {
            ContactFormStateMetaInfo Meta;

            Meta.Id = Row["Id"].as<uint32_t>();
            Meta.FormID = UUID::FromString(Row["FormID"].c_str());
            std::strncpy(Meta.StateMessage.data(), Row["StateMessage"].c_str(), Meta.StateMessage.size());

            List.push_back(Meta);
        }

        Tx.commit();
        return List;
    }

    std::vector<ContactFormInfo> GetContactForms(
        pqxx::connection &Connection,
        const ContactFormQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueId, "
            "PhoneNumber, "
            "Email, "
            "Message, "
            "Type, "
            "EXTRACT(EPOCH FROM CreatedAt)::BIGINT AS CreatedAt, "
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
            Params.push_back(std::to_string(static_cast<int>(*Query.Type)));
        }

        if (Query.State)
        {
            Sql += " AND State = $" + std::to_string(Params.size() + 1);
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
            Contact.UniqueId = UUID::FromString(Row["UniqueId"].c_str());

            std::strncpy(Contact.PhoneNumber.data(), Row["PhoneNumber"].c_str(), Contact.PhoneNumber.size());
            std::strncpy(Contact.Email.data(), Row["Email"].c_str(), Contact.Email.size());
            std::strncpy(Contact.Message.data(), Row["Message"].c_str(), Contact.Message.size());

            Contact.Type = static_cast<ContactSubjectType>(Row["Type"].as<int>());
            Contact.State = static_cast<ContactStates>(Row["State"].as<int>());

            std::time_t T = Row["CreatedAt"].as<std::time_t>();
            Contact.CreatedAt = *std::localtime(&T);

            Contacts.push_back(Contact);
        }

        Tx.commit();
        return Contacts;
    }

    bool UpdateContactForm(
        pqxx::connection &Connection,
        const UpdateContactFormQuery &UpdateQuery)
    {
        // Must provide at least one identifier
        if (!UpdateQuery.UniqueId && !UpdateQuery.Id)
        {
            return false;
        }

        std::vector<std::string> SetClauses;
        std::vector<std::string> Params;

        auto AddParam = [&](const std::string &Value)
        {
            Params.push_back(Value);
            return "$" + std::to_string(Params.size());
        };

        if (UpdateQuery.PhoneNumber)
        {
            SetClauses.push_back("PhoneNumber = " + AddParam(*UpdateQuery.PhoneNumber));
        }

        if (UpdateQuery.Email)
        {
            SetClauses.push_back("Email = " + AddParam(*UpdateQuery.Email));
        }

        if (UpdateQuery.Message)
        {
            SetClauses.push_back("Message = " + AddParam(*UpdateQuery.Message));
        }

        if (UpdateQuery.Type)
        {
            SetClauses.push_back("Type = " + AddParam(std::to_string(static_cast<int>(*UpdateQuery.Type))));
        }

        if (UpdateQuery.State)
        {
            SetClauses.push_back("State = " + AddParam(std::to_string(static_cast<int>(*UpdateQuery.State))));
        }

        // Return early if no fields were requested to be updated
        if (SetClauses.empty())
        {
            return false;
        }

        std::string Sql = "UPDATE ContactFormInfo SET ";
        for (size_t i = 0; i < SetClauses.size(); ++i)
        {
            Sql += SetClauses[i];
            if (i + 1 < SetClauses.size())
            {
                Sql += ", ";
            }
        }

        Sql += " WHERE 1=1";

        if (UpdateQuery.UniqueId)
        {
            Sql += " AND UniqueId = " + AddParam(UpdateQuery.UniqueId->ToString());
        }

        if (UpdateQuery.Id)
        {
            Sql += " AND Id = " + AddParam(std::to_string(*UpdateQuery.Id));
        }

        pqxx::work Tx(Connection);
        pqxx::params P;
        for (const auto &Val : Params)
        {
            P.append(Val);
        }

        auto Result = Tx.exec(Sql, P);
        Tx.commit();

        return Result.affected_rows() > 0;
    }

    bool UpdateContactForm(
        pqxx::connection &Connection,
        const ContactFormInfo &Info)
    {
        pqxx::work Tx(Connection);

        // Convert char arrays to std::string safely
        std::string phone(Info.PhoneNumber.data(), strnlen(Info.PhoneNumber.data(), Info.PhoneNumber.size()));
        std::string email(Info.Email.data(), strnlen(Info.Email.data(), Info.Email.size()));
        std::string message(Info.Message.data(), strnlen(Info.Message.data(), Info.Message.size()));

        std::string Sql =
            "UPDATE ContactFormInfo SET "
            "PhoneNumber = $1, "
            "Email = $2, "
            "Message = $3, "
            "Type = $4, "
            "State = $5 "
            "WHERE UniqueId = $6";

        pqxx::params P;
        P.append(phone);
        P.append(email);
        P.append(message);
        P.append(static_cast<int>(Info.Type));
        P.append(static_cast<int>(Info.State));
        P.append(Info.UniqueId.ToString());

        auto Result = Tx.exec(Sql, P);
        Tx.commit();

        return Result.affected_rows() > 0;
    }
} // namespace NepBill
