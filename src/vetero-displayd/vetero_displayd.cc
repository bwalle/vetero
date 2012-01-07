/* {{{
 * (c) 2010, Bernhard Walle <bernhard@bwalle.de>
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
#include <cerrno>
#include <sstream>
#include <csignal>
#include <cstdio>

#include <libbw/optionparser.h>
#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>
#include <libbw/os.h>

#include "common/dbaccess.h"
#include "common/error.h"
#include "common/dataset.h"
#include "common/translation.h"
#include "vetero_displayd.h"
#include "config.h"

namespace vetero {
namespace display {

/* Signal handlers {{{ */

static volatile sig_atomic_t s_quit = false;

static void veterodisplayd_sighandler(int signal)
{
    BW_ERROR_WARNING("Signal %d (%s) received. Starting shutdown.", signal, strsignal(signal));
    s_quit = true;
}

static void veterodisplayd_sigusr1_sighandler(int signal)
{}

/* }}} */
/* VeteroDisplayd {{{ */

VeteroDisplayd::VeteroDisplayd()
    : common::VeteroApplication("vetero-displayd")
    , m_noConfigFatal(false)
    , m_serdispConnection(NULL)
    , m_display(NULL)
{}

VeteroDisplayd::~VeteroDisplayd()
{
    if (m_display)
        m_display->quit();
    delete m_display;
    delete m_serdispConnection;
}

bool VeteroDisplayd::parseCommandLine(int argc, char *argv[])
{
    bw::OptionGroup generalGroup("General Options");
    generalGroup.addOption("help", 'h', bw::OT_FLAG,
                           "Prints a help message and exits.");
    generalGroup.addOption("version", 'v', bw::OT_FLAG,
                           "Prints the version and exits.");

    bw::OptionGroup loggingGroup("Logging Options");
    loggingGroup.addOption("error-logfile", 'L', bw::OT_STRING,
                           "Use the specified file for error logging. The special values "
                           "'stderr', 'stdout' and 'syslog' are accepted.");

    bw::OptionGroup configurationGroup("Configuration Options");
    configurationGroup.addOption("configfile", 'c', bw::OT_STRING,
                                 "Use the provided configuration file instead of the default.");

    bw::OptionParser op;
    op.addOptions(generalGroup);
    op.addOptions(loggingGroup);
    op.addOptions(configurationGroup);

    // do the parsing
    if (!op.parse(argc, argv))
        return false;

    // evaluate options
    if (op.getValue("help").getFlag()) {
        op.printHelp(std::cerr, "vetero-displayd " GIT_VERSION);
        return false;
    } else if (op.getValue("version").getFlag()) {
        std::cerr << "veterod " << GIT_VERSION << std::endl;
        return false;
    }

    // error logging
    std::string errorLogfile("stderr");
    if (op.getValue("error-logfile"))
        errorLogfile = op.getValue("error-logfile").getString();
    setupErrorLogging(errorLogfile);

    // configuration
    if (op.getValue("configfile")) {
        m_configfile = op.getValue("configfile").getString();
        m_noConfigFatal = true;
    }

    return true;
}

void VeteroDisplayd::installSignalhandlers()
{
    sig_t ret = std::signal(SIGTERM, veterodisplayd_sighandler);
    if (ret == SIG_ERR)
        throw common::SystemError("Unable to install signal handler for SIGTERM", errno);

    ret = std::signal(SIGINT, veterodisplayd_sighandler);
    if (ret == SIG_ERR)
        throw common::SystemError("Unable to install signal handler for SIGINT", errno);

    ret = std::signal(SIGUSR1, veterodisplayd_sigusr1_sighandler);
    if (ret == SIG_ERR)
        throw common::SystemError("Unable to install signal handler for SIGUSR1", errno);
}

void VeteroDisplayd::readConfiguration()
{
    m_configuration.reset(new common::Configuration(m_configfile));
    if (!m_configuration->configurationRead() && m_noConfigFatal)
        throw common::ApplicationError(m_configuration->error());

    setlocale(LC_ALL, m_configuration->locale().c_str());
    setlocale(LC_NUMERIC, "C");
    bindtextdomain("vetero-displayd", INSTALL_PREFIX "/share/locale");
    textdomain("vetero-displayd");
}

void VeteroDisplayd::openDatabase()
{
    try {
        m_database.open(m_configuration->databasePath(), common::Sqlite3Database::FLAG_READONLY);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to open DB: " + std::string(err.what()) );
    }

    try {
        vetero::common::DbAccess dbAccess(&m_database);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to init DB: " + std::string(err.what()) );
    }
}

void VeteroDisplayd::openDisplay()
{
    try {
        std::string displayName = m_configuration->displayName();
        std::string displayConnection = m_configuration->displayConnection();

        m_serdispConnection = new SerdisplibConnection(displayConnection);
        m_display = new SerdisplibTextDisplay(m_serdispConnection, displayName, "");
        m_display->setCharset("utf-8");
        m_display->setLocale(m_configuration->locale());
    } catch (const DisplayError &err) {
        throw common::ApplicationError("Unable to open display: " + std::string(err.what()));
    }
}

void VeteroDisplayd::updateDisplay(const common::CurrentWeather &weather)
{
    m_display->clear();

    // Temperature
    m_display->renderText(0, 0, vetero::display::BOLD_FONT, _("Temperature"));
    m_display->renderText(0, 13, vetero::display::NORMAL_FONT,
                          "%6.1lf°C", weather.temperatureReal());
    m_display->renderText(1, 2, vetero::display::NORMAL_FONT,
                          "Min/Max %5.1f/%5.1f", weather.minTemperatureReal(),
                          weather.maxTemperatureReal());

    // Humidity
    m_display->renderText(2, 0, vetero::display::BOLD_FONT, _("Humidity"));
    m_display->renderText(2, 14, vetero::display::NORMAL_FONT,
                          "%5.0lf %%", weather.humidityReal());

    // Dew point
    m_display->renderText(3, 0, vetero::display::BOLD_FONT, _("Dew point"));
    m_display->renderText(3, 13, vetero::display::NORMAL_FONT,
                          "%6.1lf°C", weather.dewpointReal());

    // Wind
    m_display->renderText(4, 0, vetero::display::BOLD_FONT, _("Wind speed"));
    m_display->renderText(4, 11, vetero::display::NORMAL_FONT,
                          "%5.1lf km/h", weather.windSpeedReal());
    m_display->renderText(5, 6, vetero::display::NORMAL_FONT, "Max.");
    m_display->renderText(5, 11, vetero::display::NORMAL_FONT, "%5.1lf km/h",
                          weather.maxWindSpeedReal());

    // Rain
    m_display->renderText(6, 0, vetero::display::BOLD_FONT, _("Rain"));
    m_display->renderText(6, 13, vetero::display::NORMAL_FONT,
                          "%5.1lf mm", weather.rainReal());

    m_display->update();
}

void VeteroDisplayd::exec()
{
    common::DbAccess dbAccess(&m_database);

    bool new_data = true;
    while (!s_quit) {

        if (new_data) {
            common::CurrentWeather weather;

            try {
                weather = dbAccess.queryCurrentWeather();
            } catch (const vetero::common::DatabaseError &err) {
                BW_ERROR_ERR("DB error: %s", err.what());
            }

            updateDisplay(weather);
        }

        // wait for the next signal
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);

        siginfo_t info;
        struct timespec timeout = { 2, 0 };
        int err = sigtimedwait(&set, &info, &timeout);
        if (err < 0) {
            if (errno == EAGAIN) {
                new_data = false;
                continue;
            } else if (errno != EINTR)
                throw common::SystemError("Problem when waiting for signal SIGUSR1", errno);
        }

        new_data = true;
    }

    BW_DEBUG_INFO("Shutting down vetero-displayd");
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
