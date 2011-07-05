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
#include <fstream>

#include <sys/wait.h>

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
#include "childprocesswatcher.h"

namespace vetero {
namespace daemon {

/* Signal handlers {{{ */

// -------------------------------------------------------------------------------------------------
static void veterod_sighandler(int signal)
{
    BW_ERROR_WARNING("vetero Signal %d (%s) received. Terminating.", signal, strsignal(signal));
    std::exit(0);
}

// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
Veterod::Veterod()
    : m_action(ActionCollectWeatherdata)
    , m_daemonize(true)
    , m_logfile(NULL)
    , m_errorLogfile("stderr")
    , m_noConfigFatal(false)
{}

// -------------------------------------------------------------------------------------------------
Veterod::~Veterod()
{
    if (m_logfile) {
        std::fclose(m_logfile);
        m_logfile = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
bool Veterod::parseCommandLine(int argc, char *argv[])
    throw (common::ApplicationError)
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

    bw::OptionGroup actionGroups("Actions (replaces the default action to collect weatherdata)");
    actionGroups.addOption("regenerate-metadata", 'M', bw::OT_FLAG,
                                 "Regenerate all cached values in the database. This may take some time.");

    bw::OptionParser op;
    op.addOptions(generalGroup);
    op.addOptions(loggingGroup);
    op.addOptions(configurationGroup);
    op.addOptions(actionGroups);

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

    // actions
    if (op.getValue("regenerate-metadata"))
        m_action = ActionRegenerateMetadata;

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
    setupErrorLogging();

    // configuration
    if (op.getValue("configfile")) {
        m_configfile = op.getValue("configfile").getString();
        m_noConfigFatal = true;
    }

    if (op.getValue("foreground").getFlag())
        m_daemonize = false;

    return true;
}

// -------------------------------------------------------------------------------------------------
void Veterod::installSignalhandlers()
    throw (common::ApplicationError)
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

// -------------------------------------------------------------------------------------------------
void Veterod::readConfiguration()
    throw (common::ApplicationError)
{
    m_configuration.reset(new common::Configuration(m_configfile));
    if (!m_configuration->configurationRead() && m_noConfigFatal)
        throw common::ApplicationError(m_configuration->error());
}

// -------------------------------------------------------------------------------------------------
void Veterod::openDatabase()
    throw (common::ApplicationError)
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

// -------------------------------------------------------------------------------------------------
void Veterod::startDisplay()
    throw (common::ApplicationError)
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

// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
void Veterod::setupDebugLogging(const std::string &levelstring, const std::string &filename)
    throw (common::ApplicationError)
{
    bw::Debug *debugger = bw::Debug::debug();

    // log level
    bw::Debug::Level level;
    if (levelstring == "none" || levelstring == "NONE")
        level = bw::Debug::DL_NONE;
    else if (levelstring == "info" || levelstring == "INFO")
        level = bw::Debug::DL_INFO;
    else if (levelstring == "debug" || levelstring == "DEBUG")
        level = bw::Debug::DL_DEBUG;
    else if (levelstring == "trace" || levelstring == "TRACE")
        level = bw::Debug::DL_TRACE;
    else
        throw common::ApplicationError("Invalid loglevel: '" + levelstring + "'");

    debugger->setLevel(level);

    // logfile
    if (!filename.empty()) {
        m_logfile = std::fopen(filename.c_str(), "a");
        if (!m_logfile)
            throw common::SystemError(std::string("Unable to open file '" + filename + "'"), errno);

        debugger->setFileHandle(m_logfile);
    }
}

// -------------------------------------------------------------------------------------------------
void Veterod::setupErrorLogging()
    throw (common::ApplicationError)
{
    if (m_errorLogfile == "syslog")
        bw::Errorlog::configure(bw::Errorlog::LM_SYSLOG, "veterod");
    else {
        bool success = bw::Errorlog::configure(bw::Errorlog::LM_FILE, m_errorLogfile.c_str());
        if (!success)
            throw common::ApplicationError("Unable to setup error logging for '" + m_errorLogfile + "'");
    }
}

// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
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

// -------------------------------------------------------------------------------------------------
void Veterod::exec()
    throw (common::ApplicationError)
{
    switch (m_action) {
        case ActionCollectWeatherdata:
            execCollectWeatherdata();
            break;

        case ActionRegenerateMetadata:
            execRegenerateMetadata();
            break;
    }
}

// -------------------------------------------------------------------------------------------------
void Veterod::execCollectWeatherdata()
throw (common::ApplicationError)
{
    BW_DEBUG_INFO("Starting application.");

    if (m_daemonize) {
        bw::daemonize();
        createPidfile();
    }

    DataReader reader(m_configuration->serialDevice(), m_configuration->serialBaud());
    reader.openConnection();
    startDisplay();
    common::DbAccess dbAccess(&m_database);
    // don't assume we need to regenerate everything on startup
    bw::Datetime lastInserted = bw::Datetime::now();

    while (true) {
        try {
            vetero::common::UsbWde1Dataset dataset = reader.read();
            dbAccess.insertUsbWde1Dataset(dataset);
            dbAccess.updateDayStatistics(dataset.timestamp().strftime("%Y-%m-%d"));
            notifyDisplay();

            std::vector<std::string> jobs;
            jobs.push_back("current");
            jobs.push_back("day:" + dataset.timestamp().dateStr());
            if (dataset.timestamp().day() != lastInserted.day()) {
                bw::Datetime lastDay(dataset.timestamp());
                lastDay.addDays(-1);
                std::string monthStr(lastDay.strftime("%Y-%m"));
                dbAccess.updateMonthStatistics(monthStr);
                jobs.push_back("month:" + monthStr);
            }
            updateReports(jobs, true);

            lastInserted = dataset.timestamp();
        } catch (const common::ApplicationError &err) {
            BW_ERROR_ERR("%s", err.what());
        }
    }
}

// -------------------------------------------------------------------------------------------------
void Veterod::execRegenerateMetadata()
    throw (common::ApplicationError)
{
    BW_DEBUG_INFO("Regenerating metadata.");

    common::DbAccess dbAccess(&m_database);
    dbAccess.updateDayStatistics("");
    dbAccess.updateMonthStatistics("");
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
