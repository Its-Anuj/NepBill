#ifndef NEPBILL_DATA_H
#define NEPBILL_DATA_H

#include <array>
#include <string>

namespace NepBill
{
    static constexpr int kCountryCount = 16;

    static constexpr std::array<const char *, kCountryCount> Countries =
        {
            "Afghanistan",
            "Australia",
            "Bangladesh",
            "Bhutan",
            "Brazil",
            "Canada",
            "China",
            "France",
            "Germany",
            "India",
            "Japan",
            "Nepal",
            "Pakistan",
            "Russia",
            "United Kingdom",
            "United States"};
} // namespace  NepBill

#endif