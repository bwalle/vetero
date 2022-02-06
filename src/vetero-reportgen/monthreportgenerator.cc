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

#include <cassert>

#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/fileutils.h>

#include "common/utils.h"
#include "common/translation.h"
#include "common/dbaccess.h"
#include "monthreportgenerator.h"
#include "calendar.h"
#include "gnuplot.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

MonthReportGenerator::MonthReportGenerator(VeteroReportgen    *reportGenerator,
                                           const std::string  &month)
    : ReportGenerator(reportGenerator),
      m_monthString(month)
{}

void MonthReportGenerator::generateReports()
{
    try {
        if (m_monthString.empty()) {
            common::DbAccess dbAccess(&reportgen()->database());
            std::vector<std::string> dates = dbAccess.dataMonths();

            std::vector<std::string>::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(*it);
        } else
            generateOneReport(m_monthString);
    } catch (const common::DatabaseError &err) {
        throw common::ApplicationError("DB error: " + std::string(err.what()));
    }
}

void MonthReportGenerator::generateOneReport(const std::string &date)
{
    BW_DEBUG_INFO("Generating month report for %s", date.c_str());
    reset();

    m_monthString = date;

    if (m_monthString.size() != 7)
        throw common::ApplicationError("Invalid month: " + m_monthString);

    int year = bw::from_str<int>(m_monthString.substr(0, 4));
    int month = bw::from_str<int>(m_monthString.substr(5, 2));
    m_month = bw::Datetime(year, month, 1, 0, 0, 0, false);

    try {
        bw::FileUtils::mkdir(nameProvider().monthlyDir(m_month), true);
    } catch (const bw::Error &err) {
        throw common::ApplicationError(err.what());
    }

    m_firstDayStr = m_month.strftime("%Y-%m-01");
    m_lastDayStr = m_month.strftime("%Y-%m-") + bw::str(Calendar::daysPerMonth(m_month));

    createTemperatureDiagram();
    if (haveWindData())
        createWindDiagram();
    if (haveRainData())
        createRainDiagram();
    createHtml();
}

void MonthReportGenerator::createTemperatureDiagram()
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT date, temp_min, temp_max, temp_avg "
        "FROM   day_statistics_float "
        "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().monthlyDiagram(m_month, "temperature"));
    plot << "set xlabel '" << _("Day") << "'\n";
    plot << "set ylabel '" << _("Temperature [°C]") << "'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_firstDayStr << "' : '" << m_lastDayStr << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with lines title 'Min' linecolor rgb '#0022FF' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:3 with lines title 'Max' linecolor rgb '#FF0000' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:4 with lines title 'Avg' linecolor rgb '#555555' lw 2\n";
    plot.plot(result.data);
}

void MonthReportGenerator::createWindDiagram()
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT date, wind_max, wind_gust_max "
        "FROM   day_statistics_float "
        "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );
    common::Database::Result maxResult = reportgen()->database().executeSqlQuery(
        "SELECT ROUND(MAX(wind_max, wind_gust_max)) + 1, MAX(wind_gust_max) "
        "FROM   day_statistics_float "
        "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    std::string max = "0.0";
    if (maxResult.data.size() > 0 && maxResult.data.front().size() > 0)
        max = maxResult.data.front().front();
    bool haveGust = !maxResult.data.front()[1].empty();

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().monthlyDiagram(m_month, "wind"));
    plot << "set xlabel '" << _("Day") <<"'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_firstDayStr << "' : '" << m_lastDayStr << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot.addWindY();
    plot << "set yrange [0 : " << max << "]\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with impulses notitle "
            "linecolor rgb '#3C8EFF' lw 4";
    if (haveGust)
        plot << ", '" << Gnuplot::PLACEHOLDER << "' using 1:3 with points title 'Böen' "
            "pt 9 ps 1 linecolor rgb '#180076' lw 2";
    plot << "\n";

    plot.plot(result.data, haveGust ? 2 : 1);
}

void MonthReportGenerator::createRainDiagram()
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT date, rain, rain "
        "FROM   day_statistics_float "
        "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    // accumulate the rain
    double sum = 0.0;
    for (int i = 0; i < result.data.size(); ++i) {
        sum += bw::from_str<double>( result.data[i].at(2) );
        result.data[i].at(2) = bw::str(sum);
    }

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().monthlyDiagram(m_month, "rain"));
    plot << "set xlabel '" << _("Day") << "'\n";
    plot << "set ylabel '" << _("Rain [l/m²]") << "'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_firstDayStr << "' : '" << m_lastDayStr << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot << "set style fill solid 1.0 border\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:3 with boxes notitle linecolor rgb '#ADD0FF' lw 1, "
         << " '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with impulses notitle linecolor rgb '#0000FF' lw 4\n";

    plot.plot(result.data);
}

void MonthReportGenerator::createHtml()
{
    std::string filename(nameProvider().monthlyIndex(m_month));
    HtmlDocument html(reportgen());

    html.setTitle(m_month.strftime("%B %Y"));

    // navigation links

    bw::Datetime lastMonth(m_month);
    lastMonth.addDays(-1);
    bw::Datetime nextMonth(m_month);
    nextMonth.addDays(31);

    const ValidDataCache &validDataCache = reportgen()->validDataCache();

    html.setForwardNavigation(
        validDataCache.dataInMonth(nextMonth)
        ? nameProvider().monthlyDirLink(nextMonth)
            : "",
        nextMonth.strftime("%B %Y")
    );
    html.setBackwardNavigation(
        validDataCache.dataInMonth(lastMonth)
            ? nameProvider().monthlyDirLink(lastMonth)
            : "",
        lastMonth.strftime("%B %Y")
    );
    html.setUpNavigation(
        nameProvider().yearlyDirLink(m_month),
        m_month.strftime("%Y")
    );

    html.addSection(_("Temperature profile"), _("Temperature"), "temperature");
    html.img(nameProvider().monthlyDiagramLink(m_month, "temperature"));
    html.addTopLink();

    if (haveWindData()) {
        html.addSection(_("Wind speed"), _("Wind"), "wind");
        html.img(nameProvider().monthlyDiagramLink(m_month, "wind"));
        html.addTopLink();
    }

    if (haveRainData()) {
        html.addSection(_("Rain"), _("Rain"), "rain");
        html.img(nameProvider().monthlyDiagramLink(m_month, "rain"));
        html.addTopLink();
    }

    html.addSection(_("Numeric values"), _("Values"), "numeric");
    createTable(html);
    html.addTopLink();

    if (!html.write(filename))
        throw common::ApplicationError("Unable to write HTML documentation to '"+ filename +"'");
}

void MonthReportGenerator::createTable(HtmlDocument &html)
{
    struct FormatDescription {
        const char *unit;
        int        precision;
        bool       active;
    } format[] = {
        { NULL,   -1, true },               // date
        { "°C",    1, true },               // temp_avg
        { "°C",    1, true },               // temp_min
        { "°C",    1, true },               // temp_avg
        { "km/h",  1, haveWindData() },     // wind_max
        { "Bft",   0, haveWindData() },     // wind_max_beaufort
        { "km/h",  1, haveWindGust() },     // wind_gust_max
        { "Bft",   0, haveWindGust() },     // wind_gust_max_beaufort
        { "l/m²",  1, haveRainData() },     // rain
        { "l/m²",  1, haveRainData() }      // rain_month
    };

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT strftime('%%s', date), "
        "       temp_avg, "
        "       temp_min, "
        "       temp_max, "
        "       wind_max, "
        "       wind_bft_max, "
        "       wind_gust_max, "
        "       wind_gust_bft_max, "
        "       rain, "
        "       rain "
        "FROM   day_statistics_float "
        "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    // accumulate the rain
    double sum = 0.0;
    for (int i = 0; i < result.data.size(); ++i) {
        sum += bw::from_str<double>( result.data[i].at(9) );
        result.data[i].at(9) = bw::str(sum);
    }

    html << "<table border='0' bgcolor='#000000' cellspacing='1' cellpadding='0' >\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b></b></th>\n"
         << "  <th style='padding: 5px' colspan=\"3\"><b>" << _("temperature") << "</b></th>\n";

    if (haveWindData())
        html << "  <th style='padding: 5px' colspan=\"2\"><b>" << _("wind") << "</b></th>\n";

    if (haveWindGust())
        html << "  <th style='padding: 5px' colspan=\"2\"><b>" << _("wind gust") << "</b></th>\n";

    if (haveRainData())
        html << "  <th style='padding: 5px' colspan=\"2\"><b>" << _("rain") << "</b></th>\n";

    html << "</tr>\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b>" << _("date") << "</b></th>\n"
         << "  <th style='padding: 5px'><b>⌀</b></th>\n"
         << "  <th style='padding: 5px'><b>min</b></th>\n"
         << "  <th style='padding: 5px'><b>max</b></th>\n";

    if (haveWindData())
        html << "  <th style='padding: 5px' colspan=\"2\"><b>max</b></th>\n";

    if (haveWindGust())
        html << "  <th style='padding: 5px' colspan=\"2\"><b>max</b></th>\n";

    if (haveRainData()) {
        html << "  <th style='padding: 5px'><b>" << _("day") << "</b></th>\n"
             << "  <th style='padding: 5px'><b>" << _("sum") << "</b></th>\n";
    }

    html << "</tr>\n";

    std::string localeStr = reportgen()->configuration().locale();
    for (size_t i = 0; i < result.data.size(); i++) {
        html << "<tr bgcolor='#FFFFFF'>\n";

        for (size_t j = 0; j < result.data[i].size(); j++) {
            std::string value = result.data[i][j];

            if (j == 0) {
                bw::Datetime date = bw::Datetime(bw::from_str<time_t>(value));
                std::string weekday = date.strftime("%a");
                std::string dateStr = date.strftime(_("%Y-%m-%d"));
                std::string dateLink = nameProvider().dailyDirLink(date);

                html << "<td align='left' style='padding: 5px'>" << weekday << "</td>\n";
                html << "<td align='right' style='padding: 5px'>"
                     << "<a href='" << dateLink << "'>"
                     << dateStr << "</a></td>\n";

            } else {
                assert(j < BW_ARRAY_SIZE(format));
                FormatDescription *desc = &format[j];

                if (!desc->active)
                    continue;

                if (value.empty())
                    value = "--";
                else if (desc->precision > 0) {
                    double numericValue = bw::from_str<double>(value, std::locale::classic());
                    value = common::str_printf_l("%.*lf", localeStr.c_str(), desc->precision,
                                                 numericValue);
                }
                if (desc->unit)
                    value += " " + std::string(desc->unit);

                html << "<td align='right' style='padding: 5px'>" << value << "</td>\n";
            }
        }

        html << "</tr>\n";
    }

    html << "</table>\n";
}

bool MonthReportGenerator::havePressureData() const
{
    if (m_havePressure == -1)
        m_havePressure = haveWeatherData("pressure");

    return m_havePressure;
}

bool MonthReportGenerator::haveRainData() const
{
    if (m_haveRain == -1)
        m_haveRain = haveWeatherData("rain");

    return m_haveRain;
}

bool MonthReportGenerator::haveWindData() const
{
    if (m_haveWind == -1)
        m_haveWind = haveWeatherData("wind_avg");

    return m_haveWind;
}

bool MonthReportGenerator::haveWindGust() const
{
    if (m_haveGust == -1)
        m_haveGust = haveWeatherData("wind_gust_avg");

    return m_haveGust;
}

bool MonthReportGenerator::haveWeatherData(const std::string &data) const
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT   count(*) "
        "FROM     day_statistics "
        "WHERE    date BETWEEN date(?, 'localtime') AND date(?, 'localtime') AND "
        "         %s IS NOT NULL",
        m_firstDayStr.c_str(), m_lastDayStr.c_str(),
        data.c_str()
    );

    return (bw::from_str<int>(result.data.front().front()) > 0);
}

void MonthReportGenerator::reset()
{
    m_havePressure = -1;
    m_haveRain = -1;
    m_haveWind = -1;
    m_haveGust = -1;
}


} // end namespace reportgen
} // end namespace vetero
