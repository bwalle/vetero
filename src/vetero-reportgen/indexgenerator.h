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

#ifndef VETERO_REPORTGEN_INDEXGENERATOR_H_
#define VETERO_REPORTGEN_INDEXGENERATOR_H_

#include "common/dbaccess.h"
#include "reportgenerator.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Generates the index page
 *
 * This page contains calendars with links to the weather data.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class IndexGenerator: public ReportGenerator
{
    public:
        /**
         * \brief C'tor
         *
         * \param[in] reportGenerator the appliation's main class
         */
        IndexGenerator(VeteroReportgen *reportGenerator);

        /**
         * \brief Does the work.
         *
         * \exception common::ApplicationError if something failed
         */
        virtual void generateReports()
        throw (common::ApplicationError);

    protected:
        /**
         * \brief Generates the HTML for a given year
         *
         * \param[in] html the stream to which the HTML is written
         * \param[in] year the year
         */
        void generateYear(HtmlDocument &html, int year);

        /**
         * \brief Generates the HTML for a given month
         *
         * \param[in] html the stream to which the HTML is written
         * \param[in] month the bw::Datetime object representing the first day of the month
         */
        void generateMonth(HtmlDocument &html, const bw::Datetime &month);

    private:
        common::DbAccess m_dbAccess;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_INDEXGENERATOR_H_
