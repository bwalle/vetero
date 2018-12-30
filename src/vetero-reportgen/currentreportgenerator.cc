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
#include <unistd.h>

#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>
#include <libbw/stringutil.h>

#include "common/translation.h"
#include "common/dbaccess.h"
#include "common/utils.h"
#include "htmldocument.h"
#include "vetero_reportgen.h"
#include "currentreportgenerator.h"
#include "config.h"

namespace vetero {
namespace reportgen {

std::string translateWind(const std::string &englishDirection)
{
    const char *windDirs[] = {
        gettext_noop("N"),
        gettext_noop("NNE"),
        gettext_noop("NE"),
        gettext_noop("ENE"),
        gettext_noop("E"),
        gettext_noop("ESE"),
        gettext_noop("SE"),
        gettext_noop("SSE"),
        gettext_noop("S"),
        gettext_noop("SSW"),
        gettext_noop("SW"),
        gettext_noop("WSW"),
        gettext_noop("W"),
        gettext_noop("WNW"),
        gettext_noop("NW"),
        gettext_noop("NNW"),
    };

    (void) windDirs;

    return gettext(englishDirection.c_str());
}

CurrentReportGenerator::CurrentReportGenerator(VeteroReportgen *reportGenerator)
    : ReportGenerator(reportGenerator)
{}

void CurrentReportGenerator::generateReports()
{
    BW_DEBUG_INFO("Updating current_weather.svgz");

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
    std::string reportDir(reportgen()->configuration().reportDirectory());
    std::string outputfilename = reportDir + "/current_weather.svgz";

    std::ofstream output(outputfilename.c_str());
    if (!input.is_open() || !output.is_open())
        throw common::ApplicationError("Unable to open input/output file when generating SVG");

    std::string line;
    while (std::getline(input, line)) {
        std::string::size_type pos;

        std::string loc = reportgen()->configuration().locale();
        std::string value;

        pos = line.find("TT.T");
        if (pos != std::string::npos) {
            value = common::str_printf_l("%.1lf", loc.c_str(), currentWeather.temperatureReal());
            line.replace(pos, 4, value);
        }

        pos = line.find("DD.D");
        if (pos != std::string::npos) {
            if (currentWeather.hasHumidity())
                value = common::str_printf_l("%.1lf", loc.c_str(), currentWeather.dewpointReal());
            else
                value = common::dashDecimalValue(loc, 2, 1);
            line.replace(pos, 4, value);
        }

        pos = line.find("UUUU-UU-UU UU:UU");
        if (pos != std::string::npos) {
            value = currentWeather.timestamp().strftime(_("%Y-%m-%d %H:%M"));
            line.replace(pos, 16, value);
        }

        pos = line.find("HH");
        if (pos != std::string::npos) {
            if (currentWeather.hasHumidity())
                value = common::str_printf_l("%.0lf", loc.c_str(), currentWeather.humidityReal());
            else
                value = common::dashDecimalValue(loc, 2);
            line.replace(pos, 2, value);
        }

        pos = line.find("WW.W");
        if (pos != std::string::npos) {
            if (currentWeather.hasWindSpeed())
                value = common::str_printf_l("%.1lf", loc.c_str(), currentWeather.windSpeedReal());
            else
                value = common::dashDecimalValue(loc, 2, 1);
            line.replace(pos, 4, value);
        }

        pos = line.find("WB");
        if (pos != std::string::npos) {
            if (currentWeather.hasWindSpeed())
                value = common::str_printf_l("%d", loc.c_str(), currentWeather.windBeaufort());
            else
                value = common::dashDecimalValue(loc, 2);
            line.replace(pos, 2, value);
        }

        pos = line.find("WDD");
        if (pos != std::string::npos) {
            if (currentWeather.hasWindDirection())
                value = std::to_string( (180 + currentWeather.windDirection()) % 360 );
            else
                value = "0";
            line.replace(pos, 3, value);
        }

        pos = line.find("WD");
        if (pos != std::string::npos) {
            if (currentWeather.hasWindDirection())
                value = translateWind( currentWeather.windDirectionStr() );
            else
                value = "---";
            line.replace(pos, 2, value);
        }

        pos = line.find("RR.R");
        if (pos != std::string::npos) {
            if (currentWeather.hasRain())
                value = common::str_printf_l("%.1lf", loc.c_str(), currentWeather.rainReal());
            else
                value = common::dashDecimalValue(loc, 2, 1);
            line.replace(pos, 4, value);
        }

        pos = line.find("PPPP");
        if (pos != std::string::npos) {
            if (currentWeather.hasPressure())
                value = common::str_printf_l("%4.0lf", loc.c_str(), currentWeather.pressureReal());
            else
                value = common::dashDecimalValue(loc, 4);
            line.replace(pos, 4, value);
        }

        output << line;
    }

    output.close();
    common::compress_file(outputfilename);
}

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
