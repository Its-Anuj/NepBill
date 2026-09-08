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

    std::vector<ItemStockLedger> GetItemStockLedgers(
        pqxx::connection &Connection,
        const ItemStockLedgerQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "BusinessID, "
            "PurchaseOrderID, "
            "ItemId, "
            "StockDelta, "
            "AccountID, "
            "Reason, "
            "EXTRACT(EPOCH FROM CreatedAt)::BIGINT AS CreatedAt "
            "FROM ItemStockLedger "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.PurchaseOrderID)
        {
            Sql += " AND PurchaseOrderID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.PurchaseOrderID->ToString());
        }

        if (Query.ItemId)
        {
            Sql += " AND ItemId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ItemId->ToString());
        }

        if (Query.AccountID)
        {
            Sql += " AND AccountID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.AccountID->ToString());
        }

        if (Query.Reason)
        {
            Sql += " AND Reason = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<int>(*Query.Reason)));
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
        case ItemStockLedgerSortField::Id:
            Sql += "Id";
            break;
        case ItemStockLedgerSortField::CreatedAt:
            Sql += "CreatedAt";
            break;
        case ItemStockLedgerSortField::StockDelta:
            Sql += "StockDelta";
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

        std::vector<ItemStockLedger> Ledgers;

        for (auto Row : Result)
        {
            ItemStockLedger Ledger;

            Ledger.Id = Row["Id"].as<uint32_t>();
            Ledger.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            Ledger.PurchaseOrderID = UUID::FromString(Row["PurchaseOrderID"].c_str());
            Ledger.ItemId = UUID::FromString(Row["ItemId"].c_str());
            Ledger.StockDelta = Row["StockDelta"].as<int32_t>();
            Ledger.AccountID = UUID::FromString(Row["AccountID"].c_str());
            Ledger.Reason = static_cast<StockMovementReason>(Row["Reason"].as<int>());

            std::time_t T = Row["CreatedAt"].as<std::time_t>();
            Ledger.CreatedAt = *std::localtime(&T);

            Ledgers.push_back(Ledger);
        }

        Tx.commit();
        return Ledgers;
    }

    std::vector<PurchaseOrder> GetPurchaseOrders(
        pqxx::connection &Connection,
        const PurchaseOrderQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueID, "
            "BusinessID, "
            "SupplierID, "
            "ItemInvoiceId, "
            "State, "
            "EXTRACT(EPOCH FROM CreatedAt)::BIGINT AS CreatedAt "
            "FROM PurchaseOrder "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.UniqueID)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueID->ToString());
        }

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.SupplierID)
        {
            Sql += " AND SupplierID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.SupplierID->ToString());
        }

        if (Query.ItemInvoiceId)
        {
            Sql += " AND ItemInvoiceId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ItemInvoiceId->ToString());
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
        case PurchaseOrderSortField::Id:
            Sql += "Id";
            break;
        case PurchaseOrderSortField::CreatedAt:
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

        std::vector<PurchaseOrder> Orders;

        for (auto Row : Result)
        {
            PurchaseOrder Order;

            Order.Id = Row["Id"].as<uint32_t>();
            Order.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            Order.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            Order.SupplierID = UUID::FromString(Row["SupplierID"].c_str());
            Order.ItemInvoiceId = UUID::FromString(Row["ItemInvoiceId"].c_str());
            Order.State = static_cast<PurchaseOrderState>(Row["State"].as<int>());

            std::time_t T = Row["CreatedAt"].as<std::time_t>();
            Order.CreatedAt = *std::localtime(&T);

            Orders.push_back(Order);
        }

        Tx.commit();
        return Orders;
    }

    std::vector<PurchaseOrderLine> GetPurchaseOrderLines(
        pqxx::connection &Connection,
        const PurchaseOrderLineQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueID, "
            "PurchaseOrderID, "
            "ItemID, "
            "OrderedQuantity, "
            "ReceivedQuantity, "
            "PurchaseUnit, "
            "PurchaseToStockConversion, "
            "StockUnit, "
            "UnitPrice, "
            "DiscountPercent, "
            "Remarks "
            "FROM PurchaseOrderLine "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.UniqueID)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueID->ToString());
        }

        if (Query.PurchaseOrderID)
        {
            Sql += " AND PurchaseOrderID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.PurchaseOrderID->ToString());
        }

        if (Query.ItemID)
        {
            Sql += " AND ItemID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.ItemID->ToString());
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case PurchaseOrderLineSortField::Id:
            Sql += "Id";
            break;
        case PurchaseOrderLineSortField::OrderedQuantity:
            Sql += "OrderedQuantity";
            break;
        case PurchaseOrderLineSortField::UnitPrice:
            Sql += "UnitPrice";
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

        std::vector<PurchaseOrderLine> Lines;

        for (auto Row : Result)
        {
            PurchaseOrderLine LineObj{};

            LineObj.Id = Row["Id"].as<uint32_t>();
            LineObj.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            LineObj.PurchaseOrderID = UUID::FromString(Row["PurchaseOrderID"].c_str());
            LineObj.ItemID = UUID::FromString(Row["ItemID"].c_str());
            LineObj.OrderedQuantity = Row["OrderedQuantity"].as<uint32_t>();
            LineObj.ReceivedQuantity = Row["ReceivedQuantity"].as<uint32_t>();
            LineObj.PurchaseUnit = static_cast<PurchaseUnit>(Row["PurchaseUnit"].as<uint32_t>());
            LineObj.PurchaseToStockConversion = Row["PurchaseToStockConversion"].as<double>();
            LineObj.StockUnit = static_cast<MeasurementUnit>(Row["StockUnit"].as<uint32_t>());
            LineObj.UnitPrice = Row["UnitPrice"].as<double>();
            LineObj.DiscountPercent = Row["DiscountPercent"].as<double>();
            LineObj.Remarks = Row["Remarks"].as<std::string>();

            Lines.push_back(LineObj);
        }

        Tx.commit();
        return Lines;
    }

    std::vector<ItemInvoice> GetItemInvoices(
        pqxx::connection &Connection,
        const ItemInvoiceQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "SenderId, "
            "RecieverId, "
            "UniqueID, "
            "State, "
            "VatPercent, "
            "Total, "
            "SenderType, "
            "SenderServiceType, "
            "RecieverType, "
            "RecieverServiceType, "
            "CreatedAt, "
            "LastPaymentTicketDate, "
            "ClosedAt "
            "FROM ItemInvoice "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.UniqueID)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueID->ToString());
        }

        if (Query.SenderId)
        {
            Sql += " AND SenderId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.SenderId->ToString());
        }

        if (Query.RecieverId)
        {
            Sql += " AND RecieverId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.RecieverId->ToString());
        }

        if (Query.State)
        {
            Sql += " AND State = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<uint32_t>(*Query.State)));
        }

        if (Query.VatPercent)
        {
            Sql += " AND VatPercent = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.VatPercent));
        }

        if (Query.Total)
        {
            Sql += " AND Total = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Total));
        }

        if (Query.SenderType)
        {
            Sql += " AND SenderType = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<int>(Query.SenderType->Type)));

            Sql += " AND SenderServiceType = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<uint32_t>(Query.SenderType->ServiceType)));
        }

        if (Query.RecieverType)
        {
            Sql += " AND RecieverType = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<int>(Query.RecieverType->Type)));

            Sql += " AND RecieverServiceType = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(static_cast<uint32_t>(Query.RecieverType->ServiceType)));
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ItemInvoiceSortField::Id:
            Sql += "Id";
            break;

        case ItemInvoiceSortField::CreatedAt:
            Sql += "CreatedAt";
            break;

        case ItemInvoiceSortField::Total:
            Sql += "Total";
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

        std::vector<ItemInvoice> Invoices;

        for (auto Row : Result)
        {
            ItemInvoice Invoice;

            Invoice.Id = Row["Id"].as<uint32_t>();
            Invoice.SenderId = UUID::FromString(Row["SenderId"].c_str());
            Invoice.RecieverId = UUID::FromString(Row["RecieverId"].c_str());
            Invoice.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            Invoice.State = static_cast<InvoiceStates>(Row["State"].as<uint32_t>());
            Invoice.VatPercent = Row["VatPercent"].as<double>();
            Invoice.Total = Row["Total"].as<double>();

            Invoice.SenderType.Type = static_cast<decltype(Invoice.SenderType.Type)>(Row["SenderType"].as<int>());
            Invoice.SenderType.ServiceType = static_cast<decltype(Invoice.SenderType.ServiceType)>(Row["SenderServiceType"].as<uint32_t>());

            Invoice.RecieverType.Type = static_cast<decltype(Invoice.RecieverType.Type)>(Row["RecieverType"].as<int>());
            Invoice.RecieverType.ServiceType = static_cast<decltype(Invoice.RecieverType.ServiceType)>(Row["RecieverServiceType"].as<uint32_t>());

            if (!Row["CreatedAt"].is_null())
            {
                std::string CreatedAtStr = Row["CreatedAt"].c_str();
                strptime(CreatedAtStr.c_str(), "%Y-%m-%d %H:%M:%S", &Invoice.CreatedAt);
            }

            if (!Row["LastPaymentTicketDate"].is_null())
            {
                std::string LastTicketStr = Row["LastPaymentTicketDate"].c_str();
                strptime(LastTicketStr.c_str(), "%Y-%m-%d %H:%M:%S", &Invoice.LastPaymentTicketDate);
            }

            if (!Row["ClosedAt"].is_null())
            {
                std::string ClosedAtStr = Row["ClosedAt"].c_str();
                strptime(ClosedAtStr.c_str(), "%Y-%m-%d %H:%M:%S", &Invoice.ClosedAt);
            }

            Invoices.push_back(Invoice);
        }

        Tx.commit();
        return Invoices;
    }

    std::vector<Suppliers> GetSuppliers(
        pqxx::connection &Connection,
        const SuppliersQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "BusinessID, "
            "UnqiueId, "
            "Name, "
            "PhoneNumber, "
            "PanNumber, "
            "OpeningBalance "
            "FROM Suppliers "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.UnqiueId)
        {
            Sql += " AND UnqiueId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UnqiueId->ToString());
        }

        if (Query.Name && !Query.Name->empty())
        {
            if (Query.ExactMatch)
            {
                Sql += " AND Name = $" + std::to_string(Params.size() + 1);
                Params.push_back(*Query.Name);
            }
            else
            {
                Sql += " AND Name ILIKE $" + std::to_string(Params.size() + 1);
                Params.push_back("%" + *Query.Name + "%");
            }
        }

        if (Query.PhoneNumber && !Query.PhoneNumber->empty())
        {
            if (Query.ExactMatch)
            {
                Sql += " AND PhoneNumber = $" + std::to_string(Params.size() + 1);
                Params.push_back(*Query.PhoneNumber);
            }
            else
            {
                Sql += " AND PhoneNumber ILIKE $" + std::to_string(Params.size() + 1);
                Params.push_back("%" + *Query.PhoneNumber + "%");
            }
        }

        if (Query.PanNumber && !Query.PanNumber->empty())
        {
            if (Query.ExactMatch)
            {
                Sql += " AND PanNumber = $" + std::to_string(Params.size() + 1);
                Params.push_back(*Query.PanNumber);
            }
            else
            {
                Sql += " AND PanNumber ILIKE $" + std::to_string(Params.size() + 1);
                Params.push_back("%" + *Query.PanNumber + "%");
            }
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case SuppliersSortField::Id:
            Sql += "Id";
            break;
        case SuppliersSortField::Name:
            Sql += "Name";
            break;
        case SuppliersSortField::OpeningBalance:
            Sql += "OpeningBalance";
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

        std::vector<Suppliers> SupplierList;

        for (auto Row : Result)
        {
            Suppliers SupplierObj{};

            SupplierObj.Id = Row["Id"].as<uint32_t>();
            SupplierObj.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            SupplierObj.UnqiueId = UUID::FromString(Row["UnqiueId"].c_str());
            SupplierObj.Name = Row["Name"].as<std::string>();
            SupplierObj.PhoneNumber = Row["PhoneNumber"].as<std::string>();
            SupplierObj.PanNumber = Row["PanNumber"].as<std::string>();
            SupplierObj.OpeningBalance = Row["OpeningBalance"].as<double>();

            SupplierList.push_back(SupplierObj);
        }

        Tx.commit();
        return SupplierList;
    }

    std::vector<ItemCategory> GetItemCategories(
        pqxx::connection &Connection,
        const ItemCategoryQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "BusinessID, "
            "UniqueID, "
            "Name "
            "FROM ItemCategory "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.UniqueID)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueID->ToString());
        }

        if (Query.Name && !Query.Name->empty())
        {
            if (Query.ExactMatch)
            {
                Sql += " AND Name = $" + std::to_string(Params.size() + 1);
                Params.push_back(*Query.Name);
            }
            else
            {
                Sql += " AND Name ILIKE $" + std::to_string(Params.size() + 1);
                Params.push_back("%" + *Query.Name + "%");
            }
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ItemCategorySortField::Id:
            Sql += "Id";
            break;
        case ItemCategorySortField::Name:
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

        std::vector<ItemCategory> Categories;

        for (auto Row : Result)
        {
            ItemCategory Category{};

            Category.Id = Row["Id"].as<uint32_t>();
            Category.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            Category.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            Category.Name = Row["Name"].as<std::string>();

            Categories.push_back(Category);
        }

        Tx.commit();
        return Categories;
    }

    std::vector<Item> GetItems(
        pqxx::connection &Connection,
        const ItemQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "BusinessID, "
            "CategoryId, "
            "UniqueID, "
            "Name, "
            "LowStockThresold, "
            "TracksStock, "
            "Description, "
            "StockUnit, "
            "DefaultPurchaseConversion, "
            "DefaultPurchaseUnit "
            "FROM Item "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.BusinessID)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessID->ToString());
        }

        if (Query.CategoryId)
        {
            Sql += " AND CategoryId = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.CategoryId->ToString());
        }

        if (Query.UniqueID)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueID->ToString());
        }

        if (Query.Name && !Query.Name->empty())
        {
            if (Query.ExactMatch)
            {
                Sql += " AND Name = $" + std::to_string(Params.size() + 1);
                Params.push_back(*Query.Name);
            }
            else
            {
                Sql += " AND Name ILIKE $" + std::to_string(Params.size() + 1);
                Params.push_back("%" + *Query.Name + "%");
            }
        }

        if (Query.TracksStock)
        {
            Sql += " AND TracksStock = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.TracksStock ? "TRUE" : "FALSE");
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case ItemSortField::Id:
            Sql += "Id";
            break;
        case ItemSortField::Name:
            Sql += "Name";
            break;
        case ItemSortField::LowStockThresold:
            Sql += "LowStockThresold";
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

        std::vector<Item> Items;

        for (auto Row : Result)
        {
            Item ItemObj{};

            ItemObj.Id = Row["Id"].as<uint32_t>();
            ItemObj.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            ItemObj.CategoryId = UUID::FromString(Row["CategoryId"].c_str());
            ItemObj.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            ItemObj.Name = Row["Name"].as<std::string>();
            ItemObj.LowStockThresold = Row["LowStockThresold"].as<uint32_t>();
            ItemObj.TracksStock = Row["TracksStock"].as<bool>();
            ItemObj.Description = Row["Description"].as<std::string>();
            ItemObj.StockUnit = static_cast<MeasurementUnit>(Row["StockUnit"].as<uint32_t>());
            ItemObj.DefaultPurchaseConversion = Row["DefaultPurchaseConversion"].as<double>();
            ItemObj.DefaultPurchaseUnit = static_cast<PurchaseUnit>(Row["DefaultPurchaseUnit"].as<uint32_t>());

            Items.push_back(ItemObj);
        }

        Tx.commit();
        return Items;
    }

    // Update PurchaseOrder by UniqueID
    bool UpdatePurchaseOrder(
        pqxx::connection &Connection,
        const UUID &UniqueID,
        const PurchaseOrderUpdate &Update)
    {
        std::vector<std::string> Assignments;
        std::vector<std::string> Params;

        if (Update.BusinessID)
        {
            Assignments.push_back("BusinessID = $" + std::to_string(Params.size() + 1));
            Params.push_back(Update.BusinessID->ToString());
        }

        if (Update.SupplierID)
        {
            Assignments.push_back("SupplierID = $" + std::to_string(Params.size() + 1));
            Params.push_back(Update.SupplierID->ToString());
        }

        if (Update.ItemInvoiceId)
        {
            Assignments.push_back("ItemInvoiceId = $" + std::to_string(Params.size() + 1));
            Params.push_back(Update.ItemInvoiceId->ToString());
        }

        if (Update.State)
        {
            Assignments.push_back("State = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(static_cast<int>(*Update.State)));
        }

        // Nothing to update
        if (Assignments.empty())
            return false;

        std::string Sql = "UPDATE PurchaseOrder SET ";
        for (size_t i = 0; i < Assignments.size(); ++i)
        {
            Sql += Assignments[i];
            if (i + 1 < Assignments.size())
                Sql += ", ";
        }

        Sql += " WHERE UniqueID = $" + std::to_string(Params.size() + 1);
        Params.push_back(UniqueID.ToString());

        pqxx::work Tx(Connection);
        pqxx::params P;
        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);
        Tx.commit();

        return Result.affected_rows() > 0;
    }

    // Update PurchaseOrderLine by UniqueID
    bool UpdatePurchaseOrderLine(
        pqxx::connection &Connection,
        const UUID &UniqueID,
        const PurchaseOrderLineUpdate &Update)
    {
        std::vector<std::string> Assignments;
        std::vector<std::string> Params;

        if (Update.PurchaseOrderID)
        {
            Assignments.push_back("PurchaseOrderID = $" + std::to_string(Params.size() + 1));
            Params.push_back(Update.PurchaseOrderID->ToString());
        }

        if (Update.ItemID)
        {
            Assignments.push_back("ItemID = $" + std::to_string(Params.size() + 1));
            Params.push_back(Update.ItemID->ToString());
        }

        if (Update.OrderedQuantity)
        {
            Assignments.push_back("OrderedQuantity = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(*Update.OrderedQuantity));
        }

        if (Update.ReceivedQuantity)
        {
            Assignments.push_back("ReceivedQuantity = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(*Update.ReceivedQuantity));
        }

        if (Update.PurchaseUnit)
        {
            Assignments.push_back("PurchaseUnit = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(static_cast<int>(*Update.PurchaseUnit)));
        }

        if (Update.PurchaseToStockConversion)
        {
            Assignments.push_back("PurchaseToStockConversion = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(*Update.PurchaseToStockConversion));
        }

        if (Update.StockUnit)
        {
            Assignments.push_back("StockUnit = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(static_cast<int>(*Update.StockUnit)));
        }

        if (Update.UnitPrice)
        {
            Assignments.push_back("UnitPrice = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(*Update.UnitPrice));
        }

        if (Update.DiscountPercent)
        {
            Assignments.push_back("DiscountPercent = $" + std::to_string(Params.size() + 1));
            Params.push_back(std::to_string(*Update.DiscountPercent));
        }

        if (Update.Remarks)
        {
            Assignments.push_back("Remarks = $" + std::to_string(Params.size() + 1));
            Params.push_back(*Update.Remarks);
        }

        // Nothing to update
        if (Assignments.empty())
            return false;

        std::string Sql = "UPDATE PurchaseOrderLine SET ";
        for (size_t i = 0; i < Assignments.size(); ++i)
        {
            Sql += Assignments[i];
            if (i + 1 < Assignments.size())
                Sql += ", ";
        }

        Sql += " WHERE UniqueID = $" + std::to_string(Params.size() + 1);
        Params.push_back(UniqueID.ToString());

        pqxx::work Tx(Connection);
        pqxx::params P;
        for (const auto &Value : Params)
            P.append(Value);

        auto Result = Tx.exec(Sql, P);
        Tx.commit();

        return Result.affected_rows() > 0;
    }

    std::vector<RoomInfo> GetRooms(
        pqxx::connection &Connection,
        const RoomInfoQuery &Query)
    {
        pqxx::work Tx(Connection);
        std::string Sql =
            "SELECT "
            "Id, "
            "UniqueID, "
            "BusinessID, "
            "Name, "
            "BasePrice, "
            "Description, "
            "State, "
            "BedCount "
            "FROM RoomInfo "
            "WHERE 1=1";

        std::vector<std::string> Params;

        if (Query.Id)
        {
            Sql += " AND Id = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.Id));
        }

        if (Query.UniqueId)
        {
            Sql += " AND UniqueID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.UniqueId->ToString());
        }

        if (Query.BusinessId)
        {
            Sql += " AND BusinessID = $" + std::to_string(Params.size() + 1);
            Params.push_back(Query.BusinessId->ToString());
        }

        if (Query.Name)
        {
            Sql += " AND Name = $" + std::to_string(Params.size() + 1);
            Params.push_back(*Query.Name);
        }

        if (Query.State)
        {
            Sql += " AND State = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.State));
        }

        if (Query.BedCount)
        {
            Sql += " AND BedCount = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.BedCount));
        }

        if (Query.BasePrice)
        {
            Sql += " AND BasePrice = $" + std::to_string(Params.size() + 1);
            Params.push_back(std::to_string(*Query.BasePrice));
        }

        Sql += " ORDER BY ";

        switch (Query.SortField)
        {
        case RoomSortField::Id:
            Sql += "Id";
            break;

        case RoomSortField::Name:
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

        std::vector<RoomInfo> Rooms;

        for (auto Row : Result)
        {
            RoomInfo Room;

            Room.Id = Row["Id"].as<uint32_t>();
            Room.UniqueID = UUID::FromString(Row["UniqueID"].c_str());
            Room.BusinessID = UUID::FromString(Row["BusinessID"].c_str());
            Room.Name = Row["Name"].c_str();
            Room.BasePrice = Row["BasePrice"].as<double>();
            Room.Description = Row["Description"].c_str();
            Room.State = static_cast<RoomStates>(Row["State"].as<uint32_t>());
            Room.BedCount = Row["BedCount"].as<uint32_t>();

            Rooms.push_back(Room);
        }

        Tx.commit();
        return Rooms;
    }
}