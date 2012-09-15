/* {{{
 * (c) 2012, Bernhard Walle <bernhard@bwalle.de>
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

#ifndef VETERO_REPORTGEN_YEARREPORTGENERATOR_H_
#define VETERO_REPORTGEN_YEARREPORTGENERATOR_H_

#include <libbw/datetime.h>

#include "reportgenerator.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

/**
 * \brief Creates the year statistics HTML page including the diagrams
 *
 * Generates the year statistics page, including a diagram for the temperature, the
 * humidity, the wind and the rain.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class YearReportGenerator : public ReportGenerator
{
    public:
        /**
         * \brief Creates a new instance
         *
         * \param[in] reportGenerator the appliation's main class
         * \param[in] year the month in ISO format (<tt>YYYY</tt>)
         */
        YearReportGenerator(VeteroReportgen    *reportGenerator,
                             const std::string  &year);

        /**
         * \brief Does the work.
         *
         * \exception common::ApplicationError if something failed
         */
        virtual void generateReports();

    protected:
        void generateOneReport(const std::string &year);
        void createTemperatureDiagram();
        void createRainDiagram();
        void createHtml();
        void createTable(HtmlDocument &html);
        bool haveRainData() const;

        std::string buildxticksMonths() const;

    private:
        std::string m_yearString;
        bw::Datetime m_year;

        std::string m_firstDayStr;
        std::string m_lastDayStr;

        // 0=false, 1=true, -1=not set
        mutable int m_haveRain;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_YEARREPORTGENERATOR_H_
