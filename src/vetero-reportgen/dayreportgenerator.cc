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

#include <cstdlib>

#include <libbw/log/debug.h>

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
    BW_DEBUG_INFO("Generating daily report for %s",
                  m_date.empty() ? "every day" : m_date.c_str());

    try {
        if (m_date.empty()) {
            common::Database::DbResultVector dates = reportgen()->database().executeSqlQuery(
                    "SELECT     DISTINCT date(timestamp) AS d "
                    "FROM       weatherdata "
                    "ORDER BY   d");

            common::Database::DbResultVector::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(it->at(0));
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
    m_temperatureFileName = m_date + "_temperature.svg";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT time(timestamp), temp, dewpoint "
            "FROM   weatherdata_extended "
            "WHERE  date(timestamp) = ?", m_date.c_str());

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel \"Zeit [HH:MM]\"\n";
    plot << "set ylabel \"Temperatur [°C]\"\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "set output '" << m_temperatureFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with lines title 'Temperatur' linecolor rgb '#CC0000' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER << "' using 1:3 with lines title 'Taupunkt' linecolor rgb '#FF8500' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createHumidityDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating humidity diagrams for %s", m_date.c_str());
    m_humidityFileName = m_date + "_humidity.svg";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT time(timestamp), humid "
            "FROM   weatherdata_extended "
            "WHERE  date(timestamp) = ?", m_date.c_str());

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel \"Zeit [HH:MM]\"\n";
    plot << "set ylabel \"Luftfeuchtigkeit [%]\"\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "set output '" << m_humidityFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with lines notitle linecolor rgb '#3C8EFF' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createWindDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating wind diagrams for %s", m_date.c_str());
    m_windFileName = m_date + "_wind.svg";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT time(timestamp), wind "
            "FROM   weatherdata_extended "
            "WHERE  date(timestamp) = ?", m_date.c_str());
    common::Database::DbResultVector maxResult = reportgen()->database().executeSqlQuery(
            "SELECT ROUND(MAX(wind)) "
            "FROM   weatherdata_extended "
            "WHERE  date(timestamp) = ?", m_date.c_str());

    std::string max = maxResult.at(0).at(0);

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel \"Zeit [HH:MM]\"\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot.addWindY();
    plot << "set yrange [0 : " << max << "]\n";
    plot << "set output '" << m_windFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with points notitle  pt 7 ps 1 "
            "linecolor rgb '#180076' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createRainDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    BW_DEBUG_DBG("Generating rain diagrams for %s", m_date.c_str());
    m_rainFileName = m_date + "_rain.svg";

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT time(timestamp), rain "
            "FROM   weatherdata_extended "
            "WHERE  date(timestamp) = ?", m_date.c_str());

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel \"Zeit [HH:MM]\"\n";
    plot << "set ylabel \"Niederschlag [mm]\"\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";

    // there might be days with no rain :-)
    if (result.empty() || result.back().empty() || std::atof(result.back().back().c_str()) < 0.001)
        plot << "set yrange [0:1]\n";
    else
        plot << "set yrange [0:]\n";

    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set style fill solid 1.0 border\n";
    plot << "set output '" << m_rainFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with boxes notitle linecolor rgb '#ADD0FF' lw 2\n";

    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void DayReportGenerator::createHtml()
    throw (common::ApplicationError, common::DatabaseError)
{
    std::string filename(reportgen()->configuration().getReportDirectory() + "/" + m_date + ".xhtml");

    int year = std::atoi( m_date.substr(0, 4).c_str() );
    int month = std::atoi( m_date.substr(5, 2).c_str() );
    int day = std::atoi( m_date.substr(8, 2).c_str() );

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
