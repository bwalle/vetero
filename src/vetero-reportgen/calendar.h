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

#ifndef CALENDAR_H_
#define CALENDAR_H_

#include <string>

namespace vetero {
namespace reportgen {

/**
 * @brief Simple helper class with static methods with some Calendar information
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */
class Calendar
{
    public:
        /**
         * @brief Returns the number of days in the given month
         *
         * @param[in] year the 4-digit year, e.g. 2010
         * @param[in] month the month where 1 = January and 12 = December
         * @return the number of days between 28 and 31.
         */
        static int daysPerMonth(int year, int month);

        /**
         * @brief Returns the locale-specific day abbreviation
         *
         * The global locale is used.
         *
         * @param[in] wday the weekday according to enum Weekday
         * @return the two or three letter abbreviation, e.g. <tt>"Mo"</tt> for Monday.
         */
        static std::string dayAbbreviation(int wday);

        /**
         * @brief Returns the locale-specific month name
         *
         * @param[in] month the month from 1 to 12
         * @return the month name, e.g. <tt>"Juni"</tt> for 6 and a German locale.
         */
        static std::string monthName(int month);

        /**
         * @brief Checks if @p year is a leap year
         *
         * @param[in] year the 4-digit year, e.g. 2010
         * @return @c true if the year is a leap year, @c false otherwise
         */
        static bool isLeapYear(int year);
};

} // namespace reportgen
} // namespace vetero

#endif /* CALENDAR_H_ */
