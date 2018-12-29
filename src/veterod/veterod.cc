/* {{{
 * (c) 2010-2012, Bernhard Walle <bernhard@bwalle.de>
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
#include <fstream>

#include <sys/wait.h>
#include <unistd.h>

#include <libbw/optionparser.h>
#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>
#include <libbw/os.h>

#include "common/dbaccess.h"
#include "common/utils.h"
#include "veterod.h"
#include "config.h"
#include "datareader.h"
#include "pressurereader.h"
#include "childprocesswatcher.h"

namespace vetero {
namespace daemon {

/* Signal handlers {{{ */

static void veterod_sighandler(int signal)
{
    BW_ERROR_WARNING("vetero Signal %d (%s) received. Terminating.", signal, strsignal(signal));
    std::exit(0);
}

static pid_t s_displayPid;
static void quit_display_daemon()
{
    if (!s_displayPid)
        return;

    int err = kill(s_displayPid, SIGTERM);
    if (err < 0)
        BW_ERROR_WARNING("Unable to kill vetero-displayd (%d): %s", s_displayPid, strerror(errno));
}

/* }}} */
/* Veterod {{{ */

Veterod::Veterod()
    : common::VeteroApplication("veterod"),
      m_daemonize(true),
      m_errorLogfile("stderr"),
      m_noConfigFatal(false)
{}

bool Veterod::parseCommandLine(int argc, char *argv[])
{
    bw::OptionGroup generalGroup("General Options");
    generalGroup.addOption("help", 'h', bw::OT_FLAG,
                           "Prints a help message and exits.");
    generalGroup.addOption("version", 'v', bw::OT_FLAG,
                           "Prints the version and exits.");
    generalGroup.addOption("foreground", 'f', bw::OT_FLAG,
                           "Don't fork (run in foreground)");

    bw::OptionGroup loggingGroup("Logging Options");
    loggingGroup.addOption("debug-logfile", 'D', bw::OT_STRING,
                           "Don't log to the console, log in FILE instead");
    loggingGroup.addOption("debug-loglevel", 'd', bw::OT_STRING,
                           "Specify the loglevel ('none'*, 'info', 'debug', trace')");
    loggingGroup.addOption("error-logfile", 'L', bw::OT_STRING,
                           "Use the specified file for error logging. The special values "
                           "'stderr', 'stdout' and 'syslog' are accepted.");

    bw::OptionGroup configurationGroup("Configuration Options");
    configurationGroup.addOption("configfile", 'c', bw::OT_STRING,
                                 "Use the provided configuration file rather than '" + m_configfile + "'");

    bw::OptionParser op;
    op.addOptions(generalGroup);
    op.addOptions(loggingGroup);
    op.addOptions(configurationGroup);

    // do the parsing
    if (!op.parse(argc, argv))
        return false;

    // evaluate options
    if (op.getValue("help").getFlag()) {
        op.printHelp(std::cerr, "veterod " GIT_VERSION);
        return false;
    } else if (op.getValue("version").getFlag()) {
        std::cerr << "veterod " << GIT_VERSION << std::endl;
        return false;
    }

    // debug logging
    std::string debugLoglevel("none");
    std::string debugLogfile;
    if (op.getValue("debug-loglevel"))
        debugLoglevel = op.getValue("debug-loglevel").getString();
    if (op.getValue("debug-logfile"))
        debugLogfile = op.getValue("debug-logfile").getString();
    setupDebugLogging(debugLoglevel, debugLogfile);

    // error logging
    if (op.getValue("error-logfile"))
        m_errorLogfile = op.getValue("error-logfile").getString();
    setupErrorLogging(m_errorLogfile);

    // configuration
    if (op.getValue("configfile")) {
        m_configfile = op.getValue("configfile").getString();
        m_noConfigFatal = true;
    }

    if (op.getValue("foreground").getFlag())
        m_daemonize = false;

    return true;
}

void Veterod::installSignalhandlers()
{
    BW_DEBUG_DBG("Registering signal handler for SIGTERM");
    sig_t ret = std::signal(SIGTERM, veterod_sighandler);
    if (ret == SIG_ERR)
        throw common::SystemError("Unable to install signal handler", errno);

    BW_DEBUG_DBG("Registering signal handler for SIGINT");
    ret = std::signal(SIGINT, veterod_sighandler);
    if (ret == SIG_ERR)
        throw common::SystemError("Unable to install signal handler", errno);

    atexit(quit_display_daemon);
}

void Veterod::readConfiguration()
{
    m_configuration.reset(new common::Configuration(m_configfile));
    if (!m_configuration->configurationRead() && m_noConfigFatal)
        throw common::ApplicationError(m_configuration->error());
}

void Veterod::openDatabase()
{
    std::string dbPath = m_configuration->databasePath();
    bool initNeeded = access(dbPath.c_str(), F_OK) != 0;

    try {
        m_database.open(dbPath, 0);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to open DB: " + std::string(err.what()) );
    }

    try {
        vetero::common::DbAccess dbAccess(&m_database);

        if (initNeeded) {
            BW_DEBUG_INFO("Database doesn't exist, creating tables...");
            dbAccess.initTables();
        }
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to init DB: " + std::string(err.what()) );
    }
}

void Veterod::startDisplay()
{
    if (m_configuration->displayName().empty() || m_configuration->displayConnection().empty()) {
        BW_DEBUG_INFO("'display_name' or 'display_connection' not set. Not starting displayd.");
        return;
    }

    std::vector<std::string> args;
    if (!m_configfile.empty()) {
        args.push_back("--configfile");
        args.push_back(m_configfile);
    }
    args.push_back("--error-logfile");
    args.push_back(m_errorLogfile);

    s_displayPid = common::start_background("vetero-displayd", args);
    BW_DEBUG_DBG("Display daemon started with PID %ld", long(s_displayPid));
}

void Veterod::updateReports(const std::vector<std::string> &jobs, bool upload)
{
    if (m_configuration->reportDirectory().empty()) {
        BW_DEBUG_INFO("'report_directory' not set. Updating of reports disabled.");
        return;
    }

    BW_DEBUG_INFO("Updating weather reports (%s)", bw::str(jobs.begin(), jobs.end()).c_str());

    std::vector<std::string> args;
    if (!m_configfile.empty()) {
        args.push_back("--configfile");
        args.push_back(m_configfile);
    }
    args.push_back("--error-logfile");
    args.push_back(m_errorLogfile);

    if (upload)
        args.push_back("--upload");

    args.insert(args.end(), jobs.begin(), jobs.end());

    pid_t childpid = 0;
    try {
        childpid = common::start_background("vetero-reportgen", args);
        BW_DEBUG_DBG("'vetero-reportgen' started with PID %ld", long(childpid));
        ChildProcessWatcher::instance()->addChild(childpid);
    } catch (const common::ApplicationError &err) {
        BW_ERROR_ERR("updateReports: %s", err.what());
    }
}

void Veterod::notifyDisplay()
{
    if (s_displayPid == 0)
        return;

    // check if the process is still running

    int status;
    int err = waitpid(s_displayPid, &status, WNOHANG);
    if (err < 0) {
        BW_ERROR_ERR("Unable to determine status of display (pid=%d): %s",
                     s_displayPid, std::strerror(errno));
        return;
    } else if (err > 0) {
        BW_ERROR_ERR("vetero-displayd terminated with status %d", WEXITSTATUS(status));
        s_displayPid = 0;
        return;
    }

    err = kill(s_displayPid, SIGUSR1);
    if (err < 0) {
        BW_ERROR_ERR("Unable to send SIGUSR1 to %d: %s", s_displayPid, strerror(errno));
        s_displayPid = 0; // don't try again
    }
}

bool Veterod::havePressureSensor() const
{
    return (m_configuration->pressureSensorI2cBus() >= 0) &&
           (m_configuration->pressureHeight() >= 0);
}

void Veterod::updateEnvironment(const vetero::common::Dataset &dataset, int rainValue)
{
    if (!getenv("VETERO_DB"))
        setenv("VETERO_DB", m_configuration->databasePath().c_str(), true);

    if (dataset.sensorType().hasTemperature())
        setenv("VETERO_CURRENT_TEMPERATURE", bw::str(dataset.temperature()/100.0).c_str(), true);
    else
        unsetenv("VETERO_CURRENT_TEMPERATURE");

    if (dataset.sensorType().hasHumidity())
        setenv("VETERO_CURRENT_HUMIDITY", bw::str(dataset.humidity()/100.0).c_str(), true);
    else
        unsetenv("VETERO_CURRENT_HUMIDITY");

    if (dataset.sensorType().hasRain())
        setenv("VETERO_CURRENT_RAIN", bw::str(rainValue/1000.0).c_str(), true);
    else
        unsetenv("VETERO_CURRENT_RAIN");

    if (dataset.sensorType().hasWindSpeed())
        setenv("VETERO_CURRENT_WIND", bw::str(dataset.windSpeed()/100.0).c_str(), true);
    else
        unsetenv("VETERO_CURRENT_WIND");
}

void Veterod::runPostscript(const vetero::common::Dataset &dataset, int rainValue)
{
    std::string script = m_configuration->updatePostscript();

    if (script.empty())
        return;

    updateEnvironment(dataset, rainValue);
    int rc = system(script.c_str());
    if (rc != 0)
        BW_ERROR_ERR("Unable to run '%s': %d", script.c_str(), WEXITSTATUS(rc));
}

void Veterod::createPidfile()
{
    const std::string pidfile = "/var/run/veterod.pid";
    std::ofstream fout(pidfile.c_str());
    if (!fout.is_open()) {
        BW_ERROR_ERR("Unable to open file '%s' for writing: %s", pidfile.c_str(),
                     std::strerror(errno));
        return;
    }

    fout << getpid();
}

void Veterod::exec()
{
    BW_DEBUG_INFO("Starting application.");

    if (m_daemonize) {
        bw::daemonize(bw::DAEMONIZE_NOCLOSE); // SQLite is already open!
        createPidfile();
    }

    std::unique_ptr<DataReader> reader( DataReader::create(*m_configuration.get() ) );
    reader->openConnection();
    PressureReader pressureReader(m_configuration->pressureSensorI2cBus());
    pressureReader.setHeight(m_configuration->pressureHeight());
    startDisplay();
    common::DbAccess dbAccess(&m_database);
    // don't assume we need to regenerate everything on startup
    bw::Datetime lastInserted = bw::Datetime::now();

    while (true) {
        try {
            vetero::common::Dataset dataset = reader->read();
            int rainValue;

            // do some sanity check before inserting in the DB
            // Normally all values are corrupted, so it's okay to check just the temperature.
            if (dataset.temperature() < -5000 || dataset.temperature() >= 7000) {
                BW_ERROR_WARNING("Invalid dataset read, skipping (temperature: %lf)\n",
                                 dataset.temperature()/100.0);
                continue;
            }

            dbAccess.insertDataset(dataset, rainValue);
            runPostscript(dataset, rainValue);

            try {
                if (havePressureSensor())
                    dbAccess.insertPressure(pressureReader.readPressure());
            } catch (const common::ApplicationError &err) {
                BW_ERROR_WARNING("Unable to read pressure: %s", err.what());
            }

            dbAccess.updateDayStatistics(dataset.timestamp().strftime("%Y-%m-%d"));
            notifyDisplay();

            std::vector<std::string> jobs;
            jobs.push_back("current");
            jobs.push_back("day:" + dataset.timestamp().dateStr());
            if (dataset.timestamp().day() != lastInserted.day()) {

                bw::Datetime timestamp(dataset.timestamp());
                bw::Datetime lastDay(timestamp);
                lastDay.addDays(-1);
                
                //
                // update statistics
                //

                dbAccess.updateMonthStatistics(timestamp.strftime("%Y-%m"));
                if (timestamp.month() != lastDay.month())
                    dbAccess.updateMonthStatistics(lastDay.strftime("%Y-%m"));

                //
                // update reports
                //

                // last day and last month because of the next link
                jobs.push_back("day:" + lastDay.strftime("%Y-%m-%d"));
                if (timestamp.month() != lastDay.month())
                    jobs.push_back("month:" + lastDay.strftime("%Y-%m"));

                // current month to avoid dead links although there's no data yet
                jobs.push_back("month:" + timestamp.strftime("%Y-%m"));

                // update the year report each day
                jobs.push_back("year:" + lastDay.strftime("%Y"));
                if ( (timestamp.month() == bw::Datetime::January) && (timestamp.day() == 1) )
                    jobs.push_back("year:" + timestamp.strftime("%Y"));
            }
            updateReports(jobs, true);

            lastInserted = dataset.timestamp();
        } catch (const common::ApplicationError &err) {
            BW_ERROR_ERR("%s", err.what());
        }
    }
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
