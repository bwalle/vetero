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

#ifndef VETERO_REPORTGEN_VALIDDATACACHE_H_
#define VETERO_REPORTGEN_VALIDDATACACHE_H_

#include <string>
#include <vector>

#include "common/error.h"
#include "common/dbaccess.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Generating diagrams with Gnuplot
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class ValidDataCache
{
    public:
        /**
         * \brief C'tor
         *
         * Creates a new instance of ValidDataCache.
         *
         * \param[in] dbAccess a reference to a database access object
         * \exception common::DatabaseError if retrieving information from the DB failed
         */
        ValidDataCache(common::DbAccess &dbAccess)
        throw (common::DatabaseError);

        /**
         * \brief Virtual d'tor
         */
        virtual ~ValidDataCache();

    public:
        /**
         * \brief Checks if there is data available at the given day
         *
         * \param[in] year the 4-digit year
         * \param[in] month the month from 1 to 12
         * \param[in] day the day from 1 to 31
         * \return \c true if there's weather data available, \c false otherwise
         */
        bool dataAtDay(int year, int month, int day) const;

        /**
         * \brief Checks if there is data available at the given month
         *
         * \param[in] year the 4-digit year
         * \param[in] month the month from 1 to 12
         * \return \c true if there's weather data available, \c false otherwise
         */
        bool dataInMonth(int year, int month) const;

    private:
        common::DbAccess &m_dbAccess;
        std::vector<std::string> m_dataMonths;
        std::vector<std::string> m_dataDays;
};

} // namespace reportgen
} // namespace vetero

#endif // VETERO_REPORTGEN_VALIDDATACACHE_H_
