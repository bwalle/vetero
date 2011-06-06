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

#ifndef INDEXGENERATOR_H_
#define INDEXGENERATOR_H_

#include "reportgenerator.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

/**
 * @brief Generates the index page
 *
 * This page contains calendars with links to the weather data.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 */
class IndexGenerator: public ReportGenerator
{
    public:
        /**
         * @brief C'tor
         *
         * @param[in] reportGenerator the appliation's main class
         */
        IndexGenerator(VeteroReportgen *reportGenerator);

        /**
         * @brief Does the work.
         *
         * @exception common::ApplicationError if something failed
         */
        virtual void generateReports()
        throw (common::ApplicationError);

    protected:
        /**
         * @brief Generates the HTML for a given year
         *
         * @param[in] html the stream to which the HTML is written
         * @param[in] year the year
         */
        void generateYear(HtmlDocument &html, int year);

        /**
         * @brief Generates the HTML for a given month
         *
         * @param[in] html the stream to which the HTML is written
         * @param[in] year the year
         * @param[in] month the month
         */
        void generateMonth(HtmlDocument &html, int year, int month);

        /**
         * @brief Checks if there is data available at the given day
         *
         * @param[in] year the 4-digit year
         * @param[in] month the month from 1 to 12
         * @param[in] day the day from 1 to 31
         * @return @c true if there's weather data available, @c false otherwise
         */
        bool dataAtDay(int year, int month, int day);

        /**
         * @brief Checks if there is data available at the given month
         *
         * @param[in] year the 4-digit year
         * @param[in] month the month from 1 to 12
         * @return the number of days which have data in the given month
         */
        int dataInMonth(int year, int month);

        /**
         * @brief Retrieves the year range in which data is available
         *
         * @param[out] firstYear the first year for which data is available (as 4-digit year)
         * @param[out] lastYear the last year for which data is available (as 4-digit year)
         */
        void getYearRange(int &firstYear, int &lastYear);
};

} // end namespace reportgen
} // end namespace vetero

#endif /* INDEXGENERATOR_H_ */
