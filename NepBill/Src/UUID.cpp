#include "UUID.h"

#include <chrono>
#include <random>
#include <cstring>

namespace NepBill
{
    static std::random_device RandomDevice;
    static std::mt19937_64 Engine(RandomDevice());

    UUID::UUID()
    {
        uint64_t timestamp =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();

        uint64_t randA = Engine();
        uint64_t randB = Engine();

        uint64_t high =
            ((timestamp & 0xFFFFFFFFFFFFULL) << 16) |
            (0x7ULL << 12) |
            (randA & 0xFFFULL);

        uint64_t low =
            (0x2ULL << 62) |
            (randB & 0x3FFFFFFFFFFFFFFFULL);

        std::memcpy(Id + 0, &high, sizeof(high));
        std::memcpy(Id + 8, &low, sizeof(low));
    }
}