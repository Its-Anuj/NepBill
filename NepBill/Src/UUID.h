#ifndef NEPBILL_SHARED_UUID_H
#define NEPBILL_SHARED_UUID_H

#include <functional> // if you're using std::hash>
#include <string>
#include <cstring>
#include <string_view>
#include <stdint.h> // if you're using std::hash>
// ^-- Remove the semicolon that was here

namespace NepBill
{
    class UUID
    {
    public:
        UUID();
        UUID(const UUID &) = default;

        const uint8_t *Data() const { return Id; }
        uint8_t *Data() { return Id; }

        bool operator==(const UUID &Other) const
        {
            return std::memcmp(Id, Other.Id, 16) == 0;
        }

        // Convert UUID to a standard PostgreSQL string format: "00000000-0000-0000-0000-000000000000"
        std::string ToString() const
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');

            for (int i = 0; i < 16; ++i)
            {
                ss << std::setw(2) << static_cast<int>(Id[i]);

                // Insert standard PostgreSQL hyphens at bytes 4, 6, 8, and 10
                if (i == 3 || i == 5 || i == 7 || i == 9)
                {
                    ss << '-';
                }
            }
            return ss.str();
        }

        // Safely parse a PostgreSQL UUID string with full validation
        static UUID FromString(std::string_view sv)
        {
            // 1. Filter out hyphens to get pure hex digits
            std::string hexStr;
            hexStr.reserve(32);
            for (char c : sv)
            {
                if (c != '-')
                {
                    hexStr.push_back(static_cast<char>(std::tolower(c)));
                }
            }

            // 2. Validate length (Must be exactly 32 hex characters)
            if (hexStr.length() != 32)
            {
                return UUID::InvalidUUID();
            }

            // 3. Validate characters (Must all be 0-9 or a-f)
            bool isValidHex = std::all_of(hexStr.begin(), hexStr.end(), [](char c)
                                          { return std::isxdigit(c); });

            if (!isValidHex)
            {
                return UUID::InvalidUUID();
            }

            // 4. Parse hex characters into the 16-byte internal array
            UUID result;
            for (size_t i = 0; i < 16; ++i)
            {
                std::string byteString = hexStr.substr(i * 2, 2);
                result.Id[i] = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
            }

            return result;
        }

        bool IsInvalid() const
        {
            UUID InvalidID;
            InvalidID.MakeInvalidUUID();

            return std::memcmp(Id, InvalidID.Data(), 16) == 0;
        }

        static UUID InvalidUUID()
        {
            UUID ReturnID;
            ReturnID.MakeInvalidUUID();
            return ReturnID;
        }

        static uint32_t GetBytesSize()
        {
            return 16;
        }

    private:
        void MakeInvalidUUID()
        {
            for (int i = 0; i < 16; i++)
            {
                Id[i] = UINT8_MAX;
            }
        }

        uint8_t Id[16] = {0};
    };
} // namespace Chilli

namespace std
{
    template <>
    struct hash<NepBill::UUID>
    {
        size_t operator()(const NepBill::UUID &uuid) const
        {
            const uint64_t *data =
                reinterpret_cast<const uint64_t *>(uuid.Data());

            return std::hash<uint64_t>{}(data[0]) ^
                   (std::hash<uint64_t>{}(data[1]) << 1);
        }
    };
} // namespace std

#endif

