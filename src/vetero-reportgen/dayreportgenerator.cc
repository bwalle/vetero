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
#include <libbw/fileutils.h>
#include <libbw/stringutil.h>

#include "common/translation.h"
#include "common/utils.h"
#include "common/dbaccess.h"
#include "dayreportgenerator.h"
#include "gnuplot.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

DayReportGenerator::DayReportGenerator(VeteroReportgen      *reportGenerator,
                                       const std::string    &date)
    : ReportGenerator(reportGenerator),
      m_dateString(date)
  {}

void DayReportGenerator::generateReports()
{
    try {
        if (m_dateString.empty()) {
            common::DbAccess dbAccess(&reportgen()->database());
            std::vector<std::string> dates = dbAccess.dataDays();

            std::vector<std::string>::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(*it);
        } else
            generateOneReport(m_dateString);
    } catch (const common::DatabaseError &err) {
        throw common::ApplicationError("DB error: " + std::string(err.what()));
    }
}

void DayReportGenerator::generateOneReport(const std::string &date)
{
    BW_DEBUG_INFO("Generating daily report for %s", date.c_str());
    reset();
    m_dateString = date;

    if (m_dateString.size() != 10)
        throw common::ApplicationError("Invalid date: '" + m_dateString + "'");

    int year = bw::from_str<int>(m_dateString.substr(0, 4));
    int month = bw::from_str<int>(m_dateString.substr(5, 2));
    int day = bw::from_str<int>(m_dateString.substr(8, 2));

    m_date = bw::Datetime(year, month, day, 0, 0, 0, false);

    try {
        bw::FileUtils::mkdir(nameProvider().dailyDir(m_date), true);
    } catch (const bw::Error &err) {
        throw common::ApplicationError(err.what());
    }

    createTemperatureDiagram();
    if (haveHumidityData())
        createHumidityDiagram();
    if (haveWindData())
        createWindDiagram();
    if (haveRainData())
        createRainDiagram();
    if (haveSolarRadiationData())
        createSolarRadiationDiagram();
    if (havePressureData())
        createPressureDiagram();

    createHtml();
}

void DayReportGenerator::createTemperatureDiagram()
{
    BW_DEBUG_DBG("Generating temperature diagrams for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), temp, dewpoint "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?) "
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "temperature"));
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

    plot.plot(result.data);
}

void DayReportGenerator::createHumidityDiagram()
{
    BW_DEBUG_DBG("Generating humidity diagrams for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), humid "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?)"
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "humidity"));
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

    plot.plot(result.data);
}

void DayReportGenerator::createWindDiagram()
{
    BW_DEBUG_DBG("Generating wind diagrams for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), wind, IFNULL(wind_gust, -1.0) "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?) "
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );
    common::Database::Result maxResult = reportgen()->database().executeSqlQuery(
        "SELECT ROUND(MAX(wind_max, wind_gust_max)) + 1, MAX(wind_gust_max) "
        "FROM   day_statistics_float "
        "WHERE  date = ?", m_dateString.c_str()
    );

    std::string max = "0.0";
    if (maxResult.data.size() > 0 && maxResult.data.front().size() > 0)
        max = maxResult.data.front()[0];
    bool haveGust = !maxResult.data.front()[1].empty();

    BW_DEBUG_TRACE("haveGust=%d", !!haveGust);

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "wind"));
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
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 notitle with lines  "
            "linecolor rgb '#3C8EFF' lw 2";
    if (haveGust)
        plot << ", '" << Gnuplot::PLACEHOLDER << "' using 1:3 with points title 'Böen' "
            "pt 9 ps 1 linecolor rgb '#180076' lw 2";
    plot << "\n";

    plot.plot(result.data, haveGust ? 2 : 1);
}

void DayReportGenerator::createRainDiagram()
{
    BW_DEBUG_DBG("Generating rain diagrams for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), rain "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?)"
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );

    // accumulate the rain
    double sum = 0.0;
    for (int i = 0; i < result.data.size(); ++i) {
        sum += bw::from_str<double>( result.data[i].at(1) );
        result.data[i].at(1) = bw::str(sum);
    }

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "rain"));
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

    plot.plot(result.data);
}

void DayReportGenerator::createSolarRadiationDiagram()
{
    BW_DEBUG_DBG("Solar radiation diagram for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), solar_radiation "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?) "
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "solar"));
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "set ylabel '" << _("Solar radiation [W/m²]") << "'\n";
    plot << "set grid xtics\n";
    plot << "set ytics nomirror\n";
    plot << "set yrange [0 : 1200]\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with lines notitle "
            "linecolor rgb '#ff9900' lw 2";
    plot << "\n";

    plot.plot(result.data);
}

void DayReportGenerator::createPressureDiagram()
{
    BW_DEBUG_DBG("Generating pressure diagrams for %s", m_dateString.c_str());

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   time(timestamp), pressure "
        "FROM     weatherdata_float "
        "WHERE    jdate = julianday(?)"
        "         AND pressure > 0 "
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().dailyDiagram(m_date, "pressure"));
    plot << "set xlabel '"<< _("Time [HH:MM]") << "'\n";
    plot << "set ylabel '" << _("Air pressure [hPa]") << "'\n";
    plot << "set format x '%H:%M'\n";
    plot << "set grid\n";
    plot << "set timefmt '%H:%M:%S'\n";
    plot << "set xdata time\n";
    plot << "set xrange ['00:00:00' : '24:00:00']\n";
    plot << "set xtics format '%H:%M'\n";
    plot << "set xtics '02:00'\n";
    plot << "set yrange [960 : 1050]\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with lines notitle linecolor rgb '#ff0000' lw 2\n";

    plot.plot(result.data);
}

void DayReportGenerator::createHtml()
{
    std::string filename(nameProvider().dailyIndex(m_date));

    HtmlDocument html(reportgen());
    html.setAutoReload(5);
    html.setTitle(m_date.strftime("%A, %d. %B %Y"));

    // navigation links

    bw::Datetime yesterday(m_date);
    yesterday.addDays(-1);
    bw::Datetime tomorrow(m_date);
    tomorrow.addDays(1);

    const ValidDataCache &validDataCache = reportgen()->validDataCache();

    html.setForwardNavigation(
        validDataCache.dataAtDay(tomorrow)
            ? nameProvider().dailyDirLink(tomorrow)
            : "",
        tomorrow.strftime("%A, %d. %B %Y")
    );
    html.setBackwardNavigation(
         validDataCache.dataAtDay(yesterday)
            ? nameProvider().dailyDirLink(yesterday)
            : "",
         yesterday.strftime("%A, %d. %B %Y")
    );
    html.setUpNavigation(
        nameProvider().monthlyDirLink(m_date),
        m_date.strftime("%B %Y")
    );

    html.addSection(_("Temperature profile"), _("Temperature"), "temperature");
    html.img(nameProvider().dailyDiagramLink(m_date, "temperature"));
    html.addTopLink();

    if (haveHumidityData()) {
        html.addSection(_("Humidity profile"), _("Humidity"), "humidity");
        html.img(nameProvider().dailyDiagramLink(m_date, "humidity"));
        html.addTopLink();
    }

    if (haveWindData()) {
        html.addSection(_("Wind speed profile"), _("Wind"), "wind");
        html.img(nameProvider().dailyDiagramLink(m_date, "wind"));
        html.addTopLink();
    }

    if (haveRainData()) {
        html.addSection(_("Rain profile"), _("Rain"), "rain");
        html.img(nameProvider().dailyDiagramLink(m_date, "rain"));
        html.addTopLink();
    }

    if (haveSolarRadiationData()) {
        html.addSection(_("Solar radiation profile"), _("Solar radiation"), "solar");
        html.img(nameProvider().dailyDiagramLink(m_date, "solar"));
        html.addTopLink();
    }

    if (havePressureData()) {
        html.addSection(_("Air pressure profile"), _("Air pressure"), "pressure");
        html.img(nameProvider().dailyDiagramLink(m_date, "pressure"));
        html.addTopLink();
    }

    if (!html.write(filename))
        throw common::ApplicationError("Unable to write HTML documentation to '"+ filename +"'");
}

bool DayReportGenerator::havePressureData() const
{
    if (m_havePressure == -1)
        m_havePressure = haveWeatherData("pressure");

    return m_havePressure;
}

bool DayReportGenerator::haveSolarRadiationData() const
{
    if (m_haveSolarRadiation == -1)
        m_haveSolarRadiation = haveWeatherData("solar_radiation");

    return m_haveSolarRadiation;
}

bool DayReportGenerator::haveHumidityData() const
{
    if (m_haveHumidity == -1)
        m_haveHumidity = haveWeatherData("humid");

    return m_haveHumidity;
}

bool DayReportGenerator::haveRainData() const
{
    if (m_haveRain == -1)
        m_haveRain = haveWeatherData("rain");

    return m_haveRain;
}

bool DayReportGenerator::haveWindData() const
{
    if (m_haveWind == -1)
        m_haveWind = haveWeatherData("wind");

    return m_haveWind;
}

bool DayReportGenerator::haveWeatherData(const std::string &data) const
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   count(*) "
        "FROM     weatherdata "
        "WHERE    jdate = julianday(?) "
        "         AND %s IS NOT NULL "
        "ORDER BY timestamp",
        m_date.strftime("%Y-%m-%d 12:00").c_str(),
        data.c_str()
    );

    return (bw::from_str<int>(result.data.front().front()) > 0);
}

void DayReportGenerator::reset()
{
    m_havePressure = -1;
    m_haveHumidity = -1;
    m_haveWind = -1;
    m_haveRain = -1;
    m_haveSolarRadiation = -1;
}

} // end namespace reportgen
} // end namespace vetero
