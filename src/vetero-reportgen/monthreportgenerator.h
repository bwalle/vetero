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

#ifndef VETERO_REPORTGEN_MONTHREPORTGENERATOR_H_
#define VETERO_REPORTGEN_MONTHREPORTGENERATOR_H_

#include <libbw/datetime.h>

#include "reportgenerator.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Creates the month statistics HTML page including the diagrams
 *
 * Generates the month statistics page, including a diagram for the temperature, the
 * humidity, the wind and the rain.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class MonthReportGenerator : public ReportGenerator
{
    public:
        /**
         * \brief Creates a new instance
         *
         * \param[in] reportGenerator the appliation's main class
         * \param[in] month the month in ISO format (<tt>YYYY-MM</tt>)
         */
        MonthReportGenerator(VeteroReportgen    *reportGenerator,
                             const std::string  &month);

        /**
         * \brief Does the work.
         *
         * \exception common::ApplicationError if something failed
         */
        virtual void generateReports();

    protected:
        /**
         * \brief Generates one report
         *
         * \param[in] month the date string
         */
        void generateOneReport(const std::string &month);

        /**
         * \brief Creates the temperature diagram for one month
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createTemperatureDiagram();

        /**
         * \brief Creates the wind diagram for one month
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createWindDiagram();

        /**
         * \brief Creates the rain diagram for one month
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createRainDiagram();

        /**
         * \brief Creates the HTML page
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createHtml();

        /**
         * \brief Creates the table with the numeric values
         *
         * \param[in] html the HTML document that is used to write the HTML table
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createTable(HtmlDocument &html);

    private:
        std::string m_monthString;
        bw::Datetime m_month;

        std::string m_firstDayStr;
        std::string m_lastDayStr;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_MONTHREPORTGENERATOR_H_
