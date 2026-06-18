#include "NepaliDateConverter.h"
#include <array>
#include <vector>
#include "bs_month_days_table.h"

namespace NepBill
{
    namespace
    {

        constexpr int MinBsYear = 1970;
        constexpr int MaxBsYear = 2100;
        constexpr GregorianDate ReferenceAdDate = {1913, 4, 13};

        int gregorianToJdn(int year, int month, int day)
        {
            int a = (14 - month) / 12;
            int y = year + 4800 - a;
            int m = month + 12 * a - 3;
            return day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
        }

        GregorianDate jdnToGregorian(int jdn)
        {
            int a = jdn + 32044;
            int b = (4 * a + 3) / 146097;
            int c = a - (146097 * b) / 4;
            int d = (4 * c + 3) / 1461;
            int e = c - (1461 * d) / 4;
            int m = (5 * e + 2) / 153;
            int day = e - (153 * m + 2) / 5 + 1;
            int month = m + 3 - 12 * (m / 10);
            int year = 100 * b + d - 4800 + (m / 10);
            return {year, month, day};
        }

        bool validateBsYear(int year)
        {
            return year >= MinBsYear && year <= MaxBsYear;
        }

        void validateBsMonth(int month)
        {
            if (month < 1 || month > 12)
            {
                throw std::range_error("BS month must be in the range 1..12");
            }
        }

        void validateBsDate(int year, int month, int day)
        {
            if (day < 1 || day > 32)
            {
                throw std::range_error("BS day must be in the range 1..32");
            }
            int monthDays = 0;
            if (validateBsYear(year))
            {
                monthDays = 0; // allow invalid data for range check later
            }
            if (month >= 1 && month <= 12)
            {
                monthDays = 0;
            }
            (void)monthDays;
        }

        const std::array<std::array<int, 2>, 12> BsMonthUpperDays = {
            std::array<int, 2>{30, 31},
            std::array<int, 2>{31, 32},
            std::array<int, 2>{31, 32},
            std::array<int, 2>{31, 32},
            std::array<int, 2>{31, 32},
            std::array<int, 2>{30, 31},
            std::array<int, 2>{29, 30},
            std::array<int, 2>{29, 30},
            std::array<int, 2>{29, 30},
            std::array<int, 2>{29, 30},
            std::array<int, 2>{29, 30},
            std::array<int, 2>{30, 31}};

        const std::vector<std::vector<int>> BsMonthPatterns = {
            {0, 1, 1, 22, 1, 3, 1, 1, 1, 3, 1, 22, 1, 3, 1, 3, 1, 22, 1, 3, 1, 19, 1, 3, 1, 1, 3, 1, 2, 2, 1, 3, 1},
            {1, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 2, 2, 2, 3, 2, 2, 2, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 2, 2, 2, 2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 1, 3, 1, 1, 2},
            {0, 1, 2, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 3, 1, 1, 2},
            {1, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 2, 2, 1, 3, 1, 2, 2, 2, 1, 2},
            {59, 1, 26, 1, 28, 1, 2, 1, 12},
            {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 2, 1, 2},
            {0, 12, 1, 3, 1, 3, 1, 5, 1, 11, 1, 3, 1, 3, 1, 18, 1, 3, 1, 3, 1, 18, 1, 3, 1, 3, 1, 27, 1, 2},
            {1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 3, 1, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 15, 2, 4},
            {0, 1, 2, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 3, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 2, 2, 2, 15, 2, 4},
            {1, 1, 3, 1, 3, 1, 14, 1, 3, 1, 1, 1, 3, 1, 14, 1, 3, 1, 3, 1, 3, 1, 18, 1, 3, 1, 3, 1, 3, 1, 14, 1, 3, 15, 1, 2, 1, 1},
            {0, 1, 1, 3, 1, 3, 1, 10, 1, 3, 1, 3, 1, 1, 1, 3, 1, 3, 1, 10, 1, 3, 1, 3, 1, 3, 1, 3, 1, 14, 1, 3, 1, 3, 1, 3, 1, 3, 1, 10, 1, 20, 1, 1, 1},
            {1, 2, 2, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 3, 1, 20, 3}};

        int getBsMonthPatternDays(int bsMonth, int yearSpan)
        {
            if (yearSpan <= 0)
            {
                return 0;
            }

            const auto &pattern = BsMonthPatterns[bsMonth - 1];
            int yearCount = 0;
            int days = 0;

            for (size_t index = 0; index < pattern.size(); ++index)
            {
                int group = pattern[index];
                if (group == 0)
                {
                    continue;
                }

                int blockIndex = static_cast<int>(index % 2);
                int blockDays = BsMonthUpperDays[bsMonth - 1][blockIndex];
                if (yearSpan > yearCount + group)
                {
                    yearCount += group;
                    days += blockDays * group;
                }
                else
                {
                    days += blockDays * (yearSpan - yearCount);
                    break;
                }
            }

            return days;
        }

        int getTotalDaysSinceMinBs(int bsYear, int bsMonth, int bsDay)
        {
            if (!validateBsYear(bsYear))
            {
                throw std::range_error("BS year is out of supported bounds");
            }
            validateBsMonth(bsMonth);
            validateBsDate(bsYear, bsMonth, bsDay);

            int days = 0;
            int targetIndex = bsYear - MinBsYear;

            // sum full years from MinBsYear up to bsYear-1
            for (int y = 0; y < targetIndex; ++y)
            {
                for (int m = 0; m < 12; ++m)
                    days += BsMonthDaysTable[y][m];
            }

            // sum months in current bsYear before bsMonth
            for (int m = 0; m < bsMonth - 1; ++m)
                days += BsMonthDaysTable[targetIndex][m];

            // apply same adjustments included in original JS code
            if (bsYear > 2085 && bsYear < 2088)
            {
                days += bsDay - 2;
            }
            else if (bsYear == 2085 && bsMonth > 5)
            {
                days += bsDay - 2;
            }
            else if (bsYear == 2081 && bsMonth == 3)
            {
                days += bsDay + 1;
            }
            else if (bsYear == 2081 && bsMonth == 12)
            {
                days += bsDay - 1;
            }
            else if (bsYear > 2088)
            {
                days += bsDay - 4;
            }
            else if (bsYear == 2088 && bsMonth > 5)
            {
                days += bsDay - 4;
            }
            else
            {
                days += bsDay;
            }

            return days;
        }

        int getBsMonthDaysInternal(int bsYear, int bsMonth)
        {
            validateBsMonth(bsMonth);
            if (!validateBsYear(bsYear))
            {
                throw std::range_error("BS year is out of supported bounds");
            }

            int index = bsYear - MinBsYear;
            if (index < 0 || index > (MaxBsYear - MinBsYear))
                throw std::range_error("BS year is out of supported bounds");

            return BsMonthDaysTable[index][bsMonth - 1];
        }

        GregorianDate getGregorianFromBs(int bsYear, int bsMonth, int bsDay)
        {
            int days = getTotalDaysSinceMinBs(bsYear, bsMonth, bsDay);
            int referenceJdn = gregorianToJdn(ReferenceAdDate.year, ReferenceAdDate.month, ReferenceAdDate.day);
            return jdnToGregorian(referenceJdn + days - 1);
        }

        NepaliDate getBsFromGregorian(const GregorianDate &adDate)
        {
            int inputJdn = gregorianToJdn(adDate.year, adDate.month, adDate.day);
            int referenceJdn = gregorianToJdn(ReferenceAdDate.year, ReferenceAdDate.month, ReferenceAdDate.day);
            int relativeDays = inputJdn - referenceJdn + 1;

            int bsYear = adDate.year + 57;
            int bsMonth = (adDate.month + 9) % 12;
            if (bsMonth == 0)
            {
                bsMonth = 12;
            }

            if (adDate.month < 4)
            {
                bsYear -= 1;
            }
            else if (adDate.month == 4)
            {
                GregorianDate firstDayOfBsYear = getGregorianFromBs(bsYear, 1, 1);
                if (adDate.day < firstDayOfBsYear.day)
                {
                    bsYear -= 1;
                }
            }

            GregorianDate bsMonthStart = getGregorianFromBs(bsYear, bsMonth, 1);
            int bsDate = 1;

            if (adDate.day >= 1 && adDate.day < bsMonthStart.day)
            {
                bsMonth = (bsMonth != 1) ? bsMonth - 1 : 12;
                int monthLength = getBsMonthDaysInternal(bsYear, bsMonth);
                bsDate = monthLength - (bsMonthStart.day - adDate.day) + 1;
            }
            else
            {
                bsDate = adDate.day - bsMonthStart.day + 1;
            }

            return {bsYear, bsMonth, bsDate};
        }

    } // namespace

    GregorianDate NepaliDateConverter::toGregorian(const NepaliDate &date)
    {
        return getGregorianFromBs(date.year, date.month, date.day);
    }

    NepaliDate NepaliDateConverter::toNepali(const GregorianDate &date)
    {
        return getBsFromGregorian(date);
    }

    tm NepaliDateConverter::toNepaliTm(const tm &date)
    {
        auto BsDate = NepaliDateConverter::toNepali(date);
        struct tm Date;
        Date = date;
        Date.tm_year = BsDate.year;
        Date.tm_mon = BsDate.month;
        Date.tm_mday = BsDate.day;
        return Date;
    }

    int NepaliDateConverter::getBsMonthDays(int bsYear, int bsMonth)
    {
        return getBsMonthDaysInternal(bsYear, bsMonth);
    }

    NepaliDate NepaliDateConverter::toNepali(const tm &date)
    {
        GregorianDate Gdate;
        Gdate.day = date.tm_mday;
        Gdate.month = date.tm_mon + 1;
        Gdate.year = date.tm_year + 1900;
        return toNepali(Gdate);
    }
} // namespace nepali_date_picker
