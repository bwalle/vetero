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
#include <fstream>

#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>
#include <libbw/stringutil.h>

#include "common/dbaccess.h"
#include "common/utils.h"
#include "htmldocument.h"
#include "vetero_reportgen.h"
#include "currentreportgenerator.h"
#include "config.h"

namespace vetero {
namespace reportgen {

// -------------------------------------------------------------------------------------------------
CurrentReportGenerator::CurrentReportGenerator(VeteroReportgen *reportGenerator)
    : ReportGenerator(reportGenerator)
{}

// -------------------------------------------------------------------------------------------------
void CurrentReportGenerator::generateReports()
    throw (common::ApplicationError)
{
    BW_DEBUG_INFO("Updating current_weather.svg");

    common::CurrentWeather currentWeather;
    try {
        common::DbAccess dbAccess(&reportgen()->database());
        currentWeather = dbAccess.queryCurrentWeather();
    } catch (const common::DatabaseError &e) {
        throw common::ApplicationError("Unable to read the current weather from the DB: " +
                                       std::string(e.what()) );
    }

    std::string templateFile = findTemplate();
    if (templateFile.empty())
        throw common::ApplicationError("Unable to find SVG template");

    std::ifstream input(templateFile.c_str());
    std::string reportDir(reportgen()->configuration().getReportDirectory());
    std::ofstream output( (reportDir + "/current_weather.svg").c_str());
    if (!input.is_open() || !output.is_open())
        throw common::ApplicationError("Unable to open input/output file when generating SVG");

    std::string line;
    while (std::getline(input, line)) {
        std::string::size_type pos;

        pos = line.find("TT.T");
        if (pos != std::string::npos)
            line.replace(pos, 4, common::str_printf_l("%.1lf", "", currentWeather.temperature()));

        pos = line.find("DD.D");
        if (pos != std::string::npos)
            line.replace(pos, 4, common::str_printf_l("%.1lf", "", currentWeather.dewpoint()));

        pos = line.find("UUUU-UU-UU UU:UU");
        if (pos != std::string::npos)
            line.replace(pos, 16, currentWeather.timestamp().strftime("%Y-%m-%d %H:%M"));

        pos = line.find("HH");
        if (pos != std::string::npos)
            line.replace(pos, 2, common::str_printf_l("%d", "", currentWeather.humidity()));

        pos = line.find("WW.W");
        if (pos != std::string::npos)
            line.replace(pos, 4, common::str_printf_l("%.1lf", "", currentWeather.windSpeed()));

        pos = line.find("WB");
        if (pos != std::string::npos)
            line.replace(pos, 2, common::str_printf_l("%d", "", currentWeather.windBeaufort()));

        pos = line.find("RR.R");
        if (pos != std::string::npos)
            line.replace(pos, 4, common::str_printf_l("%.1lf", "", currentWeather.rain()));

        output << line;
    }
}

// -------------------------------------------------------------------------------------------------
std::string CurrentReportGenerator::findTemplate() const
{
    const char *templateFiles[] = {
            "share/current_weather.svg",
            INSTALL_PREFIX "/share/current_weather.svg"
    };

    for (size_t i = 0; i < sizeof(templateFiles)/sizeof(templateFiles[0]); i++) {
        const char *file = templateFiles[i];

        if (access(file, R_OK) == 0)
            return std::string(file);
    }

    return std::string();
}


/* }}} */

} // end namespace vetero
} // end namespace reportgen
