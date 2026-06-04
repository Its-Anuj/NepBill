#ifndef NEPBILL_UUID_H
#define NEPBILL_UUID_H

#include <functional> // if you're using std::hash>
#include <string>
#include <cstring>
#include <stdint.h>   // if you're using std::hash>
// ^-- Remove the semicolon that was here

namespace NepBill
{
    class UUID
    {
    public:
        UUID();
        UUID(const UUID &) = default;

        const uint8_t *Data() const { return Id; }

        bool operator==(const UUID &Other) const
        {
            return std::memcmp(Id, Other.Id, 16) == 0;
        }

        std::string ToString() const;
        static UUID FromString(std::string_view);

    private:
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