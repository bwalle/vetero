/* {{{
 * (c) 2011, Bernhard Walle <bernhard@bwalle.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>. }}}
 */

#ifndef VETERO_REPORTGEN_CALENDAR_H_
#define VETERO_REPORTGEN_CALENDAR_H_

#include <string>

#include <libbw/datetime.h>

namespace vetero {
namespace reportgen {

/**
 * \brief Simple helper class with static methods with some Calendar information
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class Calendar
{
    public:
        /**
         * \brief Returns the number of days in the given month
         *
         * \param[in] year the 4-digit year, e.g. 2010
         * \param[in] month the month where 1 = January and 12 = December
         * \return the number of days between 28 and 31.
         */
        static int daysPerMonth(int year, int month);

        /**
         * \brief Returns the number of days in the given month
         *
         * \param[in] datetime a bw::Datetime object representing the month
         * \return the number of days between 28 and 31.
         */
        static int daysPerMonth(const bw::Datetime &datetime);

        /**
         * \brief Returns the locale-specific day abbreviation
         *
         * The global locale is used.
         *
         * \param[in] wday the weekday according to enum Weekday
         * \return the two or three letter abbreviation, e.g. <tt>"Mo"</tt> for Monday.
         */
        static std::string dayAbbreviation(int wday);

        /**
         * \brief Checks if \p year is a leap year
         *
         * \param[in] year the 4-digit year, e.g. 2010
         * \return \c true if the year is a leap year, \c false otherwise
         */
        static bool isLeapYear(int year);
};

} // namespace reportgen
} // namespace vetero

#endif // VETERO_REPORTGEN_CALENDAR_H_
