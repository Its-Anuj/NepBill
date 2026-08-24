#ifndef NEPBILL_ENCRYPTION_MANAGER_H
#define NEPBILL_ENCRYPTION_MANAGER_H

#include "Tables.h"

namespace NepBill
{
    class EncryptionManager
    {
    public:
        EncryptionManager();
        ~EncryptionManager();

        // 1. Authentication helpers
        bool createPasswordHash(const std::string &password, std::array<char, kPasswordHashLength> &hash_out);
        bool verifyPassword(const std::string &password, const std::array<char, kPasswordHashLength> &hash);

        // 2. Main workflow wrappers
        bool encryptForUser(const std::string &plaintext,
                            const std::string &user_password,
                            const UUID &user_uuid,
                            EncryptedSecret &secret_out);

        bool decryptForUser(const EncryptedSecret &secret,
                            const std::string &user_password,
                            std::string &plaintext_out);

    private:
        class Impl;
        std::unique_ptr<Impl> pimpl_;
    }; // namespace NepBill
}
#endif