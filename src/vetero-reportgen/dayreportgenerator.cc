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

#include <iostream>

#include <libbw/log/debug.h>
#include <libbw/stringutil.h>

#include "common/translation.h"
#include "common/utils.h"
#include "common/dbaccess.h"
#include "dayreportgenerator.h"
#include "gnuplot.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

// -------------------------------------------------------------------------------------------------
DayReportGenerator::DayReportGenerator(VeteroReportgen      *reportGenerator,
                                       const std::string    &date)
    : ReportGenerator(reportGenerator)
    , m_date(date)
{}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::generateReports()
    throw (common::ApplicationError)
{
    try {
        if (m_date.empty()) {
            common::DbAccess dbAccess(&reportgen()->database());
            std::vector<std::string> dates = dbAccess.dataDays();

            std::vector<std::string>::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(*it);
        } else
            generateOneReport(m_date);
    } catch (const common::DatabaseError &err) {
        throw common::ApplicationError("DB error: " + std::string(err.what()));
    }
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::generateOneReport(const std::string &date)
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_INFO("Generating daily report for %s", date.c_str());
    m_date = date;

    createTemperatureDiagram();
    createHumidityDiagram();
    createWindDiagram();
    createRainDiagram();

    createHtml();
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createTemperatureDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating temperature diagrams for %s", m_date.c_str());
    m_temperatureFileName = m_date + "_temperature.svgz";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
        "SELECT time(timestamp), temp, dewpoint "
        "FROM   weatherdata_float "
        "WHERE  date(timestamp) = ?",
        m_date.c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot.setOutputFile(m_temperatureFileName);
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set ylabel '" << _("Temperature [°C]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with lines title 'Temperatur' linecolor rgb '#CC0000' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:3 with lines title 'Taupunkt' linecolor rgb '#FF8500' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createHumidityDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating humidity diagrams for %s", m_date.c_str());
    m_humidityFileName = m_date + "_humidity.svgz";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
        "SELECT time(timestamp), humid "
        "FROM   weatherdata_float "
        "WHERE  date(timestamp) = ?", m_date.c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot.setOutputFile(m_humidityFileName);
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set ylabel '" << _("Humidity [%]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with lines notitle linecolor rgb '#3C8EFF' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createWindDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating wind diagrams for %s", m_date.c_str());
    m_windFileName = m_date + "_wind.svgz";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
        "SELECT time(timestamp), wind "
        "FROM   weatherdata_float "
        "WHERE  date(timestamp) = ?", m_date.c_str()
    );
    common::Database::DbResultVector maxResult = reportgen()->database().executeSqlQuery(
        "SELECT ROUND(wind_max) + 1 "
        "FROM   day_statistics_float "
        "WHERE  date = ?", m_date.c_str()
    );

    std::string max = "0.0";
    if (maxResult.size() > 0 && maxResult.front().size() > 0)
        max = maxResult.front().front();

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot.setOutputFile(m_windFileName);
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot.addWindY();
    plot << "set yrange [0 : " << max << "]\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with points notitle  pt 7 ps 1 "
            "linecolor rgb '#180076' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createRainDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating rain diagrams for %s", m_date.c_str());
    m_rainFileName = m_date + "_rain.svgz";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
        "SELECT time(timestamp), rain "
        "FROM   weatherdata_float "
        "WHERE  date(timestamp) = ?",
        m_date.c_str()
    );

    // accumulate the rain
    double sum = 0.0;
    for (int i = 0; i < result.size(); ++i) {
        sum += bw::from_str<double>( result[i].at(1) );
        result[i].at(1) = bw::str(sum);
    }

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot.setOutputFile(m_rainFileName);
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set ylabel '" << _("Rain [l/m²]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";

    // there might be days with no rain :-)
    if (sum < 0.001)
        plot << "set yrange [0:1]\n";
    else
        plot << "set yrange [0:]\n";

    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set style fill solid 1.0 border\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with boxes notitle linecolor rgb '#ADD0FF' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createHtml()
    throw (common::ApplicationError, common::DatabaseError)
{
    std::string filename(reportgen()->configuration().getReportDirectory() + "/" + m_date + ".xhtml");

    int year = bw::from_str<int>(m_date.substr(0, 4));
    int month = bw::from_str<int>(m_date.substr(5, 2));
    int day = bw::from_str<int>(m_date.substr(8, 2));

    HtmlDocument html(reportgen());
    html.setTitle(bw::Datetime(year, month, day, 0, 0, 0, false).strftime("%A, %d. %B %Y"));

    html.addSection("Temperaturverlauf", "Temperatur", "temperature");
    html.img(m_temperatureFileName);
    html.addTopLink();

    html.addSection("Luftfeuchtigkeitsverlauf", "Luftfeuchtigkeit", "humidity");
    html.img(m_humidityFileName);
    html.addTopLink();

    html.addSection("Verlauf der Windgeschwindigkeit", "Wind", "wind");
    html.img(m_windFileName);
    html.addTopLink();

    html.addSection("Niederschlag", "Niederschlag", "rain");
    html.img(m_rainFileName);
    html.addTopLink();

    if (!html.write(filename))
        throw common::ApplicationError("Unable to write HTML documentation to '"+ filename +"'");
}

} // end namespace reportgen
} // end namespace vetero
