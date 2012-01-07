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

#ifndef VETERO_REPORTGEN_DAYSTATISTICSREPORTGENERATOR_H_
#define VETERO_REPORTGEN_DAYSTATISTICSREPORTGENERATOR_H_

#include "common/database.h"
#include "reportgenerator.h"
#include "nameprovider.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Creates the day statistics HTML page including the diagrams
 *
 * Generates the day statistics page, including a diagram for the temperature, the
 * humidity, the wind and the rain.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class DayReportGenerator : public ReportGenerator
{
    public:
        /**
         * \brief Creates a new instance
         *
         * \param[in] reportGenerator the appliation's main class
         * \param[in] date the date in ISO format (<tt>YYYY-MM-DD</tt>)
         */
        DayReportGenerator(VeteroReportgen    *reportGenerator,
                           const std::string  &date);

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
         * \param[in] date the date string
         */
        void generateOneReport(const std::string &date);

        /**
         * \brief Creates the temperature diagram for one day
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createTemperatureDiagram();

        /**
         * \brief Creates the humidity diagram for one day
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createHumidityDiagram();

        /**
         * \brief Creates the wind diagram for one day
         *
         * \exception common::ApplicationError on general error
         * \exception common::DatabaseError if the SQL is invalid
         */
        void createWindDiagram();

        /**
         * \brief Creates the rain diagram for one day
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

    private:
        std::string m_dateString;
        bw::Datetime m_date;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_DAYSTATISTICSREPORTGENERATOR_H_
