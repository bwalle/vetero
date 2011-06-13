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

#include "common/utils.h"
#include "common/translation.h"
#include "monthreportgenerator.h"
#include "calendar.h"
#include "gnuplot.h"
#include "htmldocument.h"

namespace vetero {
namespace reportgen {

// -------------------------------------------------------------------------------------------------
MonthReportGenerator::MonthReportGenerator(VeteroReportgen    *reportGenerator,
                                           const std::string  &month)
    : ReportGenerator(reportGenerator)
    , m_monthString(month)
{}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::generateReports()
    throw (common::ApplicationError)
{
    BW_DEBUG_INFO("Generating monthly report for %s",
                  m_monthString.empty() ? "every month" : m_monthString.c_str());

    try {
        if (m_monthString.empty()) {
            common::Database::DbResultVector dates = reportgen()->database().executeSqlQuery(
                    "SELECT     DISTINCT strftime('%%Y-%%m', timestamp) AS m "
                    "FROM       weatherdata "
                    "ORDER BY   m");

            common::Database::DbResultVector::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(it->at(0));
        } else
            generateOneReport(m_monthString);
    } catch (const common::DatabaseError &err) {
        throw common::ApplicationError("DB error: " + std::string(err.what()));
    }
}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::generateOneReport(const std::string &date)
    throw (common::ApplicationError, common::DatabaseError)
{
    m_monthString = date;
    m_year = bw::from_str<int>(m_monthString.substr(0, 4));
    m_month = bw::from_str<int>(m_monthString.substr(5, 2));

    createTemperatureDiagram();
    createWindDiagram();
    createRainDiagram();
    createHtml();
}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::createTemperatureDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    m_temperatureFileName = m_monthString + "_temperature.svg";

    std::string firstDay = m_monthString + "-01";
    std::string lastDay = m_monthString + "-" + bw::str(Calendar::daysPerMonth(m_year, m_month));

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT date, temp_min, temp_max, temp_avg "
            "FROM   day_statistics "
            "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')",
            firstDay.c_str(), lastDay.c_str());


    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel '" << _("Day") << "'\n";
    plot << "set ylabel '" << _("Temperature [°C]") << "'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_monthString << "-01' : '" << m_monthString << "-" << lastDay.substr(8, 2) << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot << "set output '" << m_temperatureFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with lines title 'Min' linecolor rgb '#0022FF' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER << "' using 1:3 with lines title 'Max' linecolor rgb '#FF0000' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER << "' using 1:4 with lines title 'Avg' linecolor rgb '#555555' lw 2\n";
    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::createWindDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    m_windFileName = m_monthString + "_wind.svg";

    std::string firstDay = m_monthString + "-01";
    std::string lastDay = m_monthString + "-" + bw::str(Calendar::daysPerMonth(m_year, m_month));

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT date, wind_max "
            "FROM   day_statistics "
            "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')",
            firstDay.c_str(), lastDay.c_str());
    common::Database::DbResultVector maxResult = reportgen()->database().executeSqlQuery(
            "SELECT ROUND(MAX(wind_max)) "
            "FROM   day_statistics "
            "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')",
            firstDay.c_str(), lastDay.c_str());

    std::string max = maxResult.at(0).at(0);

    WeatherGnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel '" << _("Day") <<"'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_monthString << "-01' : '"
         << m_monthString << "-" << lastDay.substr(8, 2) << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot.addWindY();
    plot << "set yrange [0 : " << max << "]\n";
    plot << "set output '" << m_windFileName << "'\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:2 with impulses notitle "
            "linecolor rgb '#180076' lw 4;\n";
    plot.plot(result);
}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::createRainDiagram()
    throw (common::ApplicationError, common::DatabaseError)
{
    m_rainFileName = m_monthString + "_rain.svg";

    std::string firstDay = m_monthString + "-01";
    std::string lastDay = m_monthString + "-" + bw::str(Calendar::daysPerMonth(m_year, m_month));

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT date, rain, rain_month "
            "FROM   day_statistics "
            "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')",
            firstDay.c_str(), lastDay.c_str());

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().getReportDirectory());
    plot << "set xlabel '" << _("Day") << "'\n";
    plot << "set ylabel '" << _("Rain [l/m²]") << "'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m-%d'\n";
    plot << "set timefmt '%Y-%m-%d'\n";
    plot << "set xrange ['" << m_monthString << "-01' : '" << m_monthString
         << "-" << lastDay.substr(8, 2) << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%2d\\n%a\"\n";
    plot << "set xtics 86400\n";
    plot << "set output '" << m_rainFileName << "'\n";
    plot << "set style fill solid 1.0 border\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER << "' using 1:3 with boxes notitle linecolor rgb '#ADD0FF' lw 1, "
         << " '" << Gnuplot::PLACEHOLDER << "' using 1:2 with impulses notitle linecolor rgb '#0000FF' lw 4\n";

    plot.plot(result);

}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::createHtml()
    throw (common::ApplicationError, common::DatabaseError)
{
    std::string filename(reportgen()->configuration().getReportDirectory() + "/" + m_monthString + ".xhtml");
    HtmlDocument html(reportgen());

    html.setTitle(bw::Datetime(m_year, m_month, 1, 0, 0, 0, false).strftime("%B %Y"));

    html.addSection(_("Temperature profile"), _("Temperature"), "temperature");
    html.img(m_temperatureFileName);
    html.addTopLink();

    html.addSection(_("Wind speed"), _("Wind"), "wind");
    html.img(m_windFileName);
    html.addTopLink();

    html.addSection(_("Rain"), _("Rain"), "rain");
    html.img(m_rainFileName);
    html.addTopLink();

    html.addSection(_("Numeric values"), _("Values"), "numeric");
    createTable(html);
    html.addTopLink();

    if (!html.write(filename))
        throw common::ApplicationError("Unable to write HTML documentation to '"+ filename +"'");
}

// -------------------------------------------------------------------------------------------------
void MonthReportGenerator::createTable(HtmlDocument &html)
    throw (common::ApplicationError, common::DatabaseError)
{
    std::string firstDay = m_monthString + "-01";
    std::string lastDay = m_monthString + "-" + bw::str(Calendar::daysPerMonth(m_year, m_month));

    struct FormatDescription {
        const char *unit;
        int        precision;
    } format[] = {
        { NULL,   -1 },     // date
        { "°C",    1 },     // temp_avg
        { "°C",    1 },     // temp_min
        { "°C",    1 },     // temp_avg
        { "km/h",  1 },     // wind_max
        { "Bft",   0 },     // wind_max_beaufort
        { "l/m²",  1 },     // rain
        { "l/m²",  1 }      // rain_month
    };

    common::Database::DbResultVector result = reportgen()->database().executeSqlQuery(
            "SELECT strftime('%%s', date), "
            "       temp_avg, "
            "       temp_min, "
            "       temp_max, "
            "       wind_max, "
            "       wind_max_beaufort, "
            "       rain, "
            "       rain_month "
            "FROM   day_statistics "
            "WHERE  date BETWEEN date(?, 'localtime') AND date(?, 'localtime')",
            firstDay.c_str(), lastDay.c_str());

    html << "<table border='0' bgcolor='#000000' cellspacing='1' cellpadding='0' >\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b></b></th>\n"
         << "  <th style='padding: 5px' colspan=\"3\"><b>Temperatur</b></th>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b>Wind</b></th>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b>Niederschlag</b></th>\n"
         << "</tr>\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b>Datum</b></th>\n"
         << "  <th style='padding: 5px'><b>Avg.</b></th>\n"
         << "  <th style='padding: 5px'><b>Min.</b></th>\n"
         << "  <th style='padding: 5px'><b>Max.</b></th>\n"
         << "  <th style='padding: 5px' colspan=\"2\"><b>Max.</b></th>\n"
         << "  <th style='padding: 5px'><b>Tag</b></th>\n"
         << "  <th style='padding: 5px'><b>Summe</b></th>\n"
         << "</tr>\n";

    std::string localeStr = reportgen()->configuration().getLocale();
    for (size_t i = 0; i < result.size(); i++) {
        html << "<tr bgcolor='#FFFFFF'>\n";

        for (size_t j = 0; j < result[i].size(); j++) {
            std::string value = result[i][j];

            if (j == 0) {
                std::string weekday = bw::Datetime(bw::from_str<time_t>(value)).strftime("%a");
                std::string date = bw::Datetime(bw::from_str<time_t>(value)).strftime(_("%Y-%m-%d"));

                html << "<td align='left' style='padding: 5px'>" << weekday << "</td>\n";
                html << "<td align='right' style='padding: 5px'>" << date << "</td>\n";

            } else {
                assert(j < ARRAY_SIZE(format));
                FormatDescription *desc = &format[j];

                if (desc->precision > 0) {
                    double numericValue = bw::from_str<double>(value, std::locale::classic());
                    value = common::str_printf_l("%.*lf", localeStr.c_str(), desc->precision, numericValue);
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

} // end namespace reportgen
} // end namespace vetero
