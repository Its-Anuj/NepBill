#include "PCH.h"
#include "Maths.h"
#include "sodium.h"
#include "EncryptionManager.h"

namespace NepBill
{
    class EncryptionManager::Impl
    {
    public:
        Impl()
        {
            if (sodium_init() < 0)
            {
                throw std::runtime_error("Failed to initialize libsodium.");
            }
        }

        bool createPasswordHash(const std::string &password, std::array<char, kPasswordHashLength> &hash_out)
        {
            hash_out.fill(0);
            int result = crypto_pwhash_str(
                hash_out.data(),
                password.data(),
                password.size(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE);
            return (result == 0);
        }

        bool verifyPassword(const std::string &password, const std::array<char, kPasswordHashLength> &hash)
        {
            return (crypto_pwhash_str_verify(hash.data(), password.data(), password.size()) == 0);
        }

        bool encryptForUser(const std::string &plaintext,
                            const std::string &user_password,
                            const UUID &user_uuid,
                            EncryptedSecret &secret_out)
        {

            secret_out.ActualCiphertextSize = crypto_secretbox_MACBYTES + plaintext.size();
            if (secret_out.ActualCiphertextSize > kPasswordHashLength)
            {
                return false; // Plaintext payload too large for the fixed 128-byte buffer
            }

            secret_out.AccountID = user_uuid;

            // Generate random salt and unique nonce
            randombytes_buf(secret_out.KeySalt.data(), crypto_pwhash_SALTBYTES);
            randombytes_buf(secret_out.EncryptionNonce.data(), crypto_secretbox_NONCEBYTES);
            secret_out.Ciphertext.fill(0);

            // Derive temporary symmetric key into isolated volatile memory
            std::array<unsigned char, kSecretKeySize> derived_key;
            if (crypto_pwhash(derived_key.data(), derived_key.size(),
                              user_password.data(), user_password.size(),
                              secret_out.KeySalt.data(),
                              crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE,
                              crypto_pwhash_ALG_DEFAULT) != 0)
            {
                return false;
            }

            // Encrypt the payload using the derived key
            int result = crypto_secretbox_easy(
                secret_out.Ciphertext.data(),
                reinterpret_cast<const unsigned char *>(plaintext.data()),
                plaintext.size(),
                secret_out.EncryptionNonce.data(),
                derived_key.data());

            // Instantly scrub key from system RAM
            sodium_memzero(derived_key.data(), derived_key.size());

            return (result == 0);
        }

        bool decryptForUser(const EncryptedSecret &secret,
                            const std::string &user_password,
                            std::string &plaintext_out)
        {

            if (secret.ActualCiphertextSize < crypto_secretbox_MACBYTES || secret.ActualCiphertextSize > kPasswordHashLength)
            {
                return false;
            }

            // Re-derive the key using the password and the package's stored unique salt
            std::array<unsigned char, kSecretKeySize> derived_key;
            if (crypto_pwhash(derived_key.data(), derived_key.size(),
                              user_password.data(), user_password.size(),
                              secret.KeySalt.data(),
                              crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE,
                              crypto_pwhash_ALG_DEFAULT) != 0)
            {
                return false;
            }

            std::size_t plain_len = secret.ActualCiphertextSize - crypto_secretbox_MACBYTES;
            std::vector<unsigned char> decrypted_buffer(plain_len, 0);

            // Open package and verify message integrity
            int result = crypto_secretbox_open_easy(
                decrypted_buffer.data(),
                secret.Ciphertext.data(),
                secret.ActualCiphertextSize,
                secret.EncryptionNonce.data(),
                derived_key.data());

            // Instantly scrub key from system RAM
            sodium_memzero(derived_key.data(), derived_key.size());

            if (result != 0)
            {
                return false; // Authentication failed (wrong password or data manipulation)
            }

            plaintext_out.assign(decrypted_buffer.begin(), decrypted_buffer.end());
            return true;
        }
    };

    // Passthrough bindings to completely hide header files
    EncryptionManager::EncryptionManager() : pimpl_(std::make_unique<Impl>()) {}
    EncryptionManager::~EncryptionManager() = default;
    bool EncryptionManager::createPasswordHash(const std::string &p, std::array<char, kPasswordHashLength> &h) { return pimpl_->createPasswordHash(p, h); }
    bool EncryptionManager::verifyPassword(const std::string &p, const std::array<char, kPasswordHashLength> &h) { return pimpl_->verifyPassword(p, h); }
    bool EncryptionManager::encryptForUser(const std::string &pt, const std::string &pw, const UUID &id, EncryptedSecret &s) { return pimpl_->encryptForUser(pt, pw, id, s); }
    bool EncryptionManager::decryptForUser(const EncryptedSecret &s, const std::string &pw, std::string &pt) { return pimpl_->decryptForUser(s, pw, pt); }

}
