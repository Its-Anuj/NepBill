#pragma once

#include <cstddef>
#include <stdexcept>

namespace NepBill {

struct NepaliDate {
    int year;
    int month;
    int day;
};

struct GregorianDate {
    int year;
    int month;
    int day;
};

class NepaliDateConverter {
public:
    static GregorianDate toGregorian(const NepaliDate& date);
    static NepaliDate toNepali(const GregorianDate& date);
    static tm toNepaliTm(const tm& date);
    static NepaliDate toNepali(const tm& date);
    static int getBsMonthDays(int bsYear, int bsMonth);
};

} // namespace nepali_date_picker
