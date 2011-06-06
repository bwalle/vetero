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
#include <libbw/os.h>

#include <common/dbaccess.h>
#include <common/error.h>
#include <common/dataset.h>

#include "vetero_displayd.h"
#include "config.h"

namespace vetero {
namespace display {

/* Signal handlers {{{ */

// -------------------------------------------------------------------------------------------------
static void veterodisplayd_sighandler(int signal)
{
    BW_ERROR_WARNING("Signal %d (%s) received. Terminating.", signal, strsignal(signal));
    std::exit(0);
}

// -------------------------------------------------------------------------------------------------
static void veterodisplayd_sigusr1_sighandler(int signal)
{}

/* }}} */
/* VeteroDisplayd {{{ */

// -------------------------------------------------------------------------------------------------
VeteroDisplayd::VeteroDisplayd()
    : m_serdispConnection(NULL)
    , m_display(NULL)
    , m_noConfigFatal(false)
{}

// -------------------------------------------------------------------------------------------------
VeteroDisplayd::~VeteroDisplayd()
{
    if (m_display)
        m_display->quit();
    delete m_display;
    delete m_serdispConnection;
}

// -------------------------------------------------------------------------------------------------
bool VeteroDisplayd::parseCommandLine(int argc, char *argv[])
    throw (common::ApplicationError)
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
        op.printHelp(std::cerr, "vetero-displayd " VERSION);
        return false;
    } else if (op.getValue("version").getFlag()) {
        std::cerr << "veterod " << VERSION << std::endl;
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

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::installSignalhandlers()
    throw (common::ApplicationError)
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

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::readConfiguration()
    throw (common::ApplicationError)
{
    m_configuration.reset(new common::Configuration(m_configfile));
    if (!m_configuration->configurationRead() && m_noConfigFatal)
        throw common::ApplicationError(m_configuration->error());
}

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::openDatabase()
    throw (common::ApplicationError)
{
    try {
        m_database.open(m_configuration->getDatabasePath(), common::Sqlite3Database::FLAG_READONLY);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to open DB: " + std::string(err.what()) );
    }

    try {
        vetero::common::DbAccess dbAccess(&m_database);
        dbAccess.initViews();
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to init DB: " + std::string(err.what()) );
    }
}

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::openDisplay()
    throw (common::ApplicationError)
{
    try {
        std::string displayName = m_configuration->getDisplayName();
        std::string displayConnection = m_configuration->getDisplayConnection();

        m_serdispConnection = new SerdisplibConnection(displayConnection);
        m_display = new SerdisplibTextDisplay(m_serdispConnection, displayName, "");
        m_display->setCharset("utf-8");
    } catch (const DisplayError &err) {
        throw common::ApplicationError("Unable to open display: " + std::string(err.what()));
    }
}

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::setupErrorLogging(const std::string &logfile)
    throw (common::ApplicationError)
{
    if (logfile == "syslog")
        bw::Errorlog::configure(bw::Errorlog::LM_SYSLOG, "vetero-displayd");
    else {
        bool success = bw::Errorlog::configure(bw::Errorlog::LM_FILE, logfile.c_str());
        if (!success)
            throw common::ApplicationError("Unable to setup error logging for '" + logfile + "'");
    }
}

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::updateDisplay(const common::CurrentWeather &weather)
    throw (common::ApplicationError)
{
    m_display->clear();

    // Temperature
    m_display->renderText(0, 0, vetero::display::BOLD_FONT, "Temperatur");
    m_display->renderText(0, 13, vetero::display::NORMAL_FONT, "%6.1f°C", weather.temperature());
    m_display->renderText(1, 2, vetero::display::NORMAL_FONT, "Min/Max %5.1f/%5.1f",
                          weather.minTemperature(), weather.maxTemperature());

    // Humidity
    m_display->renderText(2, 0, vetero::display::BOLD_FONT, "Luftfeuchte");
    m_display->renderText(2, 14, vetero::display::NORMAL_FONT, "%5d %%", weather.humidity());

    // Dew point
    m_display->renderText(3, 0, vetero::display::BOLD_FONT, "Taupunkt");
    m_display->renderText(3, 13, vetero::display::NORMAL_FONT, "%6.1f°C", weather.dewpoint());

    // Wind
    m_display->renderText(4, 0, vetero::display::BOLD_FONT, "Windgeschw.");
    m_display->renderText(4, 11, vetero::display::NORMAL_FONT, "%5.1f km/h", weather.windSpeed());
    m_display->renderText(5, 6, vetero::display::NORMAL_FONT, "Max.");
    m_display->renderText(5, 11, vetero::display::NORMAL_FONT, "%5.1f km/h", weather.maxWindSpeed());

    // Rain
    m_display->renderText(6, 0, vetero::display::BOLD_FONT, "Niederschlag");
    m_display->renderText(6, 13, vetero::display::NORMAL_FONT, "%5.1f mm", weather.rain());

    m_display->update();
}

// -------------------------------------------------------------------------------------------------
void VeteroDisplayd::exec()
    throw (common::ApplicationError)
{
    common::DbAccess dbAccess(&m_database);

    while (true) {
        common::CurrentWeather weather;

        try {
            weather = dbAccess.queryCurrentWeather();
        } catch (const vetero::common::DatabaseError &err) {
            BW_ERROR_ERR("DB error: %s", err.what());
        }

        updateDisplay(weather);

        // wait for the next signal
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        int signal_received;
        int err = sigwait(&set, &signal_received);
        if (err < 0)
            throw common::SystemError("Problem when waiting for signal SIGUSR1", errno);
    }
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
