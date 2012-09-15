/* {{{
 * (c) 2011-2012, Bernhard Walle <bernhard@bwalle.de>
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

#include <libbw/datetime.h>

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
        ValidDataCache(common::DbAccess &dbAccess);

        /**
         * \brief Virtual d'tor
         */
        virtual ~ValidDataCache();

    public:
        /**
         * \brief Checks if there is data available at the given day
         *
         * \param[in] day the bw::Datetime object representing the day
         * \return \c true if there's weather data available, \c false otherwise
         */
        bool dataAtDay(const bw::Datetime &day) const;

        /**
         * \brief Checks if there is data available at the given month
         *
         * \param[in] month the bw::Datetime object representing the month
         * \return \c true if there's weather data available, \c false otherwise
         */
        bool dataInMonth(const bw::Datetime &month) const;

        /**
         * \brief Checks if there is data available at the given year
         *
         * \param[in] year the bw::Datetime object representing the month
         * \return \c true if there's weather data available, \c false otherwise
         */
        bool dataInYear(const bw::Datetime &year) const;

    private:
        common::DbAccess &m_dbAccess;
        std::vector<std::string> m_dataMonths;
        std::vector<std::string> m_dataDays;
        std::vector<std::string> m_dataYears;
};

} // namespace reportgen
} // namespace vetero

#endif // VETERO_REPORTGEN_VALIDDATACACHE_H_
