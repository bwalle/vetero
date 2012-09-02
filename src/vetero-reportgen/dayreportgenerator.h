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

    private:
        void generateOneReport(const std::string &date);

        void createTemperatureDiagram();
        void createHumidityDiagram();
        void createWindDiagram();
        void createRainDiagram();
        void createPressureDiagram();

        void createHtml();

        bool haveHumidityData() const;
        bool haveRainData() const;
        bool haveWindData() const;
        bool havePressureData() const;

        bool haveWeatherData(const std::string &name) const;

        void reset();

    private:
        std::string m_dateString;
        bw::Datetime m_date;

        // 0=false, 1=true, -1=not set
        mutable int m_havePressure;
        mutable int m_haveRain;
        mutable int m_haveWind;
        mutable int m_haveHumidity;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_DAYSTATISTICSREPORTGENERATOR_H_
