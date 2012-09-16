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

#include <cassert>

#include <libbw/log/debug.h>
#include <libbw/stringutil.h>
#include <libbw/fileutils.h>

#include "common/translation.h"
#include "common/utils.h"
#include "yearreportgenerator.h"
#include "gnuplot.h"
#include "calendar.h"

namespace vetero {
namespace reportgen {

YearReportGenerator::YearReportGenerator(VeteroReportgen    *reportGenerator,
                                         const std::string  &year)
    : ReportGenerator(reportGenerator),
      m_yearString(year),
      m_haveRain(-1)
{}

void YearReportGenerator::generateReports()
{
    try {
        if (m_yearString.empty()) {
            common::DbAccess dbAccess(&reportgen()->database());
            std::vector<std::string> dates = dbAccess.dataYears();

            std::vector<std::string>::const_iterator it;
            for (it = dates.begin(); it != dates.end(); ++it)
                generateOneReport(*it);
        }
        else
            generateOneReport(m_yearString);
    } catch (const common::DatabaseError &err) {
        throw common::ApplicationError("DB error: " + std::string(err.what()));
    }
}

void YearReportGenerator::generateOneReport(const std::string &date)
{
    BW_DEBUG_INFO("Generating year report for %s", date.c_str());

    m_yearString = date;

    if (m_yearString.size() != 4)
        throw common::ApplicationError("Invalid year: " + m_yearString);

    int year = bw::from_str<int>(m_yearString);
    m_year = bw::Datetime(year, bw::Datetime::January, 1, 0, 0, 0, false);

    try {
        bw::FileUtils::mkdir(nameProvider().yearlyDir(m_year), true);
    } catch (const bw::Error &err) {
        throw common::ApplicationError(err.what());
    }

    m_firstDayStr = m_year.strftime("%Y-01-01");
    m_lastDayStr = m_year.strftime("%Y-12-31");

    createTemperatureDiagram();
    if (haveRainData())
        createRainDiagram();
    createHtml();
}

void YearReportGenerator::createTemperatureDiagram()
{
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT substr(month, 6), temp_min, temp_max, temp_avg "
        "FROM   month_statistics_float "
        "WHERE  month BETWEEN strftime('%%Y-%%m', ?, 'localtime') AND strftime('%%Y-%%m', ?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().yearlyDiagram(m_year, "temperature"));
    plot << "set xlabel '" << _("Month") << "'\n";
    plot << "set ylabel '" << _("Temperature [°C]") << "'\n";
    plot << "set grid\n";
    plot << "set xrange [0.5:12.5]\n";
    plot << "set mxtics 0\n";
    plot << "set xtics " << buildxticksMonths() << "\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with linespoints title 'Min' linecolor rgb '#0022FF' lw 2 pt 7 ps 1, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:3 with linespoints title 'Max' linecolor rgb '#FF0000' lw 2 pt 7 ps 1, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:4 with linespoints title 'Avg' linecolor rgb '#555555' lw 2 pt 7 ps 1\n";
    plot.plot(result.data);
}

void YearReportGenerator::createRainDiagram()
{
    // using '-16' as date centers the boxes
    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT substr(month, 6), rain, rain "
        "FROM   month_statistics_float "
        "WHERE  month BETWEEN strftime('%%Y-%%m', ?, 'localtime') AND strftime('%%Y-%%m', ?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().yearlyDiagram(m_year, "rain"));
    plot << "set xlabel '" << _("Month") << "'\n";
    plot << "set ylabel '" << _("Rain [l/m²]") << "'\n";
    plot << "set grid\n";
    plot << "set xrange [0.5:12.5]\n";
    plot << "set mxtics 0\n";
    plot << "set xtics " << buildxticksMonths() << "\n";
    plot << "set boxwidth 0.8\n";
    plot << "set style fill solid 1.0 border\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with boxes notitle linecolor rgb '#ADD0FF' lw 1\n";

    plot.plot(result.data);
}

void YearReportGenerator::createHtml()
{
    std::string filename(nameProvider().yearlyIndex(m_year));
    HtmlDocument html(reportgen());

    html.setTitle(m_year.strftime("%Y"));

    // navigation links

    bw::Datetime lastYear(m_year);
    lastYear.addDays(-1);
    bw::Datetime nextYear(m_year);
    nextYear.addDays(366);

    const ValidDataCache &validDataCache = reportgen()->validDataCache();

    html.setForwardNavigation(
        validDataCache.dataInYear(nextYear)
        ? nameProvider().yearlyDirLink(nextYear)
            : "",
        nextYear.strftime("%Y")
    );
    html.setBackwardNavigation(
        validDataCache.dataInYear(lastYear)
            ? nameProvider().yearlyDirLink(lastYear)
            : "",
        lastYear.strftime("%Y")
    );
    html.setUpNavigation("", "");

    html.addSection(_("Temperature profile"), _("Temperature"), "temperature");
    html.img(nameProvider().yearlyDiagramLink(m_year, "temperature"));
    html.addTopLink();

    if (haveRainData()) {
        html.addSection(_("Rain"), _("Rain"), "rain");
        html.img(nameProvider().yearlyDiagramLink(m_year, "rain"));
        html.addTopLink();
    }

    html.addSection(_("Numeric values"), _("Values"), "numeric");
    createTable(html);
    html.addTopLink();

    if (!html.write(filename))
        throw common::ApplicationError("Unable to write HTML documentation to '"+ filename +"'");

}

void YearReportGenerator::createTable(HtmlDocument &html)
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
        { "l/m²",  1, haveRainData() },     // rain
    };

    common::Database::Result result = reportgen()->database().executeSqlQuery(
        "SELECT month || '-1', "
        "       temp_avg, "
        "       temp_min, "
        "       temp_max, "
        "       rain "
        "FROM   month_statistics_float "
        "WHERE  month BETWEEN strftime('%%Y-%%m', ?, 'localtime') AND strftime('%%Y-%%m', ?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    html << "<table border='0' bgcolor='#000000' cellspacing='1' cellpadding='0' >\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px'><b></b></th>\n"
         << "  <th style='padding: 5px' colspan=\"3\"><b>" << _("temperature") << "</b></th>\n";

    if (haveRainData())
        html << "  <th style='padding: 5px'><b></b></th>\n";

    html << "</tr>\n"
         << "<tr bgcolor='#FFFFFF'>\n"
         << "  <th style='padding: 5px'><b>" << _("date") << "</b></th>\n"
         << "  <th style='padding: 5px'><b>⌀</b></th>\n"
         << "  <th style='padding: 5px'><b>min</b></th>\n"
         << "  <th style='padding: 5px'><b>max</b></th>\n";

    if (haveRainData())
        html << "  <th style='padding: 5px'><b>" << _("rain") << "</b></th>\n";

    html << "</tr>\n";

    std::string localeStr = reportgen()->configuration().locale();
    for (size_t i = 0; i < result.data.size(); i++) {
        html << "<tr bgcolor='#FFFFFF'>\n";

        for (size_t j = 0; j < result.data[i].size(); j++) {
            std::string value = result.data[i][j];

            if (j == 0) {
                bw::Datetime date = bw::Datetime::strptime(value, "%Y-%m-%d");
                std::string dateStr = date.strftime(_("%B %Y"));
                std::string dateLink = nameProvider().dailyDirLink(date);

                html << "<td style='padding: 5px'><a href='" << dateLink << "'>"
                     << dateStr << "</a></td>\n";
            } else {
                assert(j < BW_ARRAY_SIZE(format));
                FormatDescription *desc = &format[j];

                if (!desc->active)
                    continue;

                if (desc->precision > 0) {
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

bool YearReportGenerator::haveRainData() const
{
    if (m_haveRain == -1) {
        common::Database::Result result = reportgen()->database().executeSqlQuery(
            "SELECT   count(*) "
            "FROM     month_statistics "
            "WHERE  month BETWEEN strftime('%%Y-%%m', ?, 'localtime') AND strftime('%%Y-%%m', ?, 'localtime')"
            "       AND rain IS NOT NULL",
            m_firstDayStr.c_str(), m_lastDayStr.c_str()
        );

        m_haveRain = (bw::from_str<int>(result.data.front().front()) > 0);
    }

    return m_haveRain;
}

std::string YearReportGenerator::buildxticksMonths() const
{
    std::ostringstream xticsStream;
    xticsStream << "(";
    for (int month = bw::Datetime::January; month <= bw::Datetime::December; month++) {
        xticsStream << "'" << Calendar::monthAbbreviation(month) << "' " << month;
        if (month != bw::Datetime::December)
            xticsStream << ", ";
    }
    xticsStream << ")";
    return xticsStream.str();
}


} // end namespace reportgen
} // end namespace vetero
