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

#include <libbw/datetime.h>

#include "calendar.h"

namespace vetero {
namespace reportgen {

// -------------------------------------------------------------------------------------------------
int Calendar::daysPerMonth(int year, int month)
{
    switch (month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return 31;

        case 2:
            return isLeapYear(year) ? 29 : 28;

        case 4:
        case 6:
        case 9:
        case 11:
            return 30;

        default:
            return -1;
    }
}

// -------------------------------------------------------------------------------------------------
int Calendar::daysPerMonth(const bw::Datetime &datetime)
{
    return daysPerMonth(datetime.year(), datetime.month());
}

// -------------------------------------------------------------------------------------------------
std::string Calendar::dayAbbreviation(int wday)
{
    // use Nov 2010 since it has Monday as first day of the month
    bw::Datetime datetime(2010, 11, wday, 0, 0, 0, false);
    return datetime.strftime("%a");
}

// -------------------------------------------------------------------------------------------------
bool Calendar::isLeapYear(int year)
{
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

} // namespace reportgen
} // namespace vetero

