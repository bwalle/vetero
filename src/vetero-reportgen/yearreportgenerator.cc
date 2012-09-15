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

#include <libbw/log/debug.h>
#include <libbw/stringutil.h>
#include <libbw/fileutils.h>

#include "common/translation.h"
#include "yearreportgenerator.h"
#include "gnuplot.h"

namespace vetero {
namespace reportgen {

YearReportGenerator::YearReportGenerator(VeteroReportgen    *reportGenerator,
                                         const std::string  &year)
    : ReportGenerator(reportGenerator),
      m_yearString(year)
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
        "SELECT month, temp_min, temp_max, temp_avg "
        "FROM   month_statistics_float "
        "WHERE  month BETWEEN strftime('%%Y-%%m', ?, 'localtime') AND strftime('%%Y-%%m', ?, 'localtime')"
        "       AND temp_min != temp_max",
        m_firstDayStr.c_str(), m_lastDayStr.c_str()
    );

    BW_DEBUG_STREAM_DBG("firstday=" << m_firstDayStr << ", lastday=" << m_lastDayStr);

    Gnuplot plot(reportgen()->configuration());
    plot.setWorkingDirectory(reportgen()->configuration().reportDirectory());
    plot.setOutputFile(nameProvider().yearlyDiagram(m_year, "temperature"));
    plot << "set xlabel '" << _("Month") << "'\n";
    plot << "set ylabel '" << _("Temperature [°C]") << "'\n";
    plot << "set grid\n";
    plot << "set xdata time\n";
    plot << "set format x '%Y-%m'\n";
    plot << "set timefmt '%Y-%m'\n";
    plot << "set xrange ['" << m_firstDayStr << "' : '" << m_lastDayStr << "']\n";
    plot << "set mxtics 0\n";
    plot << "set xtics format \"%3b\"\n";
    plot << "set xtics 2721600\n";
    plot << "plot '" << Gnuplot::PLACEHOLDER
         << "' using 1:2 with lines title 'Min' linecolor rgb '#0022FF' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:3 with lines title 'Max' linecolor rgb '#FF0000' lw 2, "
         << "'" << Gnuplot::PLACEHOLDER
         << "' using 1:4 with lines title 'Avg' linecolor rgb '#555555' lw 2\n";
    plot.plot(result.data);
}

void YearReportGenerator::createRainDiagram()
{
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
}

bool YearReportGenerator::haveRainData() const
{
    return false;
}


} // end namespace reportgen
} // end namespace vetero
