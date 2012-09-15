/* {{{
 * (c) 2011-2012, Bernhard Walle <bernhard@bwalle.de>
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
#include <clocale>

#include <libbw/optionparser.h>
#include <libbw/stringutil.h>
#include <libbw/log/errorlog.h>

#include "common/translation.h"
#include "common/dbaccess.h"
#include "config.h"
#include "vetero_reportgen.h"
#include "dayreportgenerator.h"
#include "currentreportgenerator.h"
#include "monthreportgenerator.h"
#include "yearreportgenerator.h"
#include "indexgenerator.h"

namespace vetero {
namespace reportgen {

VeteroReportgen::VeteroReportgen()
    : VeteroApplication("vetero-reportgen")
    , m_noConfigFatal(false)
    , m_upload(false)
{}

vetero::common::Sqlite3Database &VeteroReportgen::database()
{
    return m_database;
}

vetero::common::Configuration &VeteroReportgen::configuration()
{
    return *m_configuration;
}

const vetero::common::Configuration &VeteroReportgen::configuration() const
{
    return *m_configuration;
}

const ValidDataCache &VeteroReportgen::validDataCache() const
{
    return *m_validDataCache;
}

void VeteroReportgen::readConfiguration()
{
    m_configuration.reset(new common::Configuration(m_configfile));
    if (!m_configuration->configurationRead() && m_noConfigFatal)
        throw common::ApplicationError(m_configuration->error());

    setlocale(LC_ALL, m_configuration->locale().c_str());
    setlocale(LC_NUMERIC, "C");
    bindtextdomain("vetero-reportgen", INSTALL_PREFIX "/share/locale");
    textdomain("vetero-reportgen");
}

void VeteroReportgen::openDatabase()
{
    try {
        m_database.open(m_configuration->databasePath(), common::Sqlite3Database::FLAG_READONLY);
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to open DB: " + std::string(err.what()) );
    }

    m_dbAccess.reset(new common::DbAccess(&m_database));
    try {
        m_validDataCache.reset(new ValidDataCache(*m_dbAccess));
    } catch (const vetero::common::DatabaseError &err) {
        throw common::ApplicationError("Unable to init DB: " + std::string(err.what()) );
    }
}

bool VeteroReportgen::parseCommandLine(int argc, char *argv[])
{
    bw::OptionGroup generalGroup("General Options");
    generalGroup.addOption("help", 'h', bw::OT_FLAG,
                           "Prints a help message and exits.");
    generalGroup.addOption("version", 'v', bw::OT_FLAG,
                           "Prints the version and exits.");

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
                                 "Use the provided configuration file instead of the default.");
    configurationGroup.addOption("upload", 'u', bw::OT_FLAG,
                                 "Upload the reports after the generation step.");

    bw::OptionParser op;
    op.addOptions(generalGroup);
    op.addOptions(loggingGroup);
    op.addOptions(configurationGroup);

    // do the parsing
    if (!op.parse(argc, argv))
        return false;

    // evaluate options
    if (op.getValue("help").getFlag()) {
        op.printHelp(std::cerr, "vetero-reportgen " GIT_VERSION
                     " <current|day|month|year> [<date>|<month>|<year>]");
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
    std::string errorLogfile("stderr");
    if (op.getValue("error-logfile"))
        errorLogfile = op.getValue("error-logfile").getString();
    setupErrorLogging(errorLogfile);

    // configuration
    if (op.getValue("configfile")) {
        m_configfile = op.getValue("configfile").getString();
        m_noConfigFatal = true;
    }
    if (op.getValue("upload"))
        m_upload = op.getValue("upload").getFlag();

    m_jobs = op.getArgs();
    return true;
}

void VeteroReportgen::uploadReports()
{
    std::string command(m_configuration->reportUploadCommand());
    if (command.empty())
        return;

    common::LockFile lock(m_configuration->reportDirectory());
    if (!lock.lockExclusive()) {
        BW_ERROR_ERR("Unable to retrieve lock: %s", lock.error().c_str());
        return;
    }

    int ret = std::system(command.c_str());
    if (ret != 0)
        BW_ERROR_ERR("Unable to upload reports: Unable to execute '%s': Exit code %d",
                     command.c_str(), WEXITSTATUS(ret));
}

void VeteroReportgen::exec()
{
    for (std::vector<std::string>::const_iterator it = m_jobs.begin(); it != m_jobs.end(); ++it) {
        const std::string &currentJob = *it;

        std::vector<std::string> jobSplit = bw::stringsplit(currentJob, ":");
        if (!(jobSplit.size() == 1 || jobSplit.size() == 2)) {
            BW_ERROR_ERR("Job description invalid: '%s'", currentJob.c_str());
            continue;
        }

        std::string jobName = jobSplit.at(0);
        std::string jobArgument;
        if (jobSplit.size() == 2)
            jobArgument = jobSplit.at(1);

        int jobsExecuted = 0;

        if (jobName == "current" || jobName == "all") {
            jobsExecuted++;
            try {
                jobsExecuted++;
                CurrentReportGenerator(this).generateReports();
            } catch (const common::ApplicationError &err) {
                BW_ERROR_ERR("Error when executing job '%s': %s", currentJob.c_str(), err.what());
            }
        }

        if (jobName == "day" || jobName == "all") {
            jobsExecuted++;
            try {
                jobsExecuted++;
                DayReportGenerator(this, jobArgument).generateReports();
            } catch (const common::ApplicationError &err) {
                BW_ERROR_ERR("Error when executing job '%s': %s", currentJob.c_str(), err.what());
            }
        }

        if (jobName == "month" || jobName == "all") {
            jobsExecuted++;
            try {
                MonthReportGenerator(this, jobArgument).generateReports();
            } catch (const common::ApplicationError &err) {
                BW_ERROR_ERR("Error when executing job '%s': %s", currentJob.c_str(), err.what());
            }

            try {
                IndexGenerator(this).generateReports();
            } catch (const common::ApplicationError &err) {
                BW_ERROR_ERR("Error when executing job '%s': %s", currentJob.c_str(), err.what());
            }
        }

        if (jobName == "year" || jobName == "all") {
            jobsExecuted++;
            try {
                YearReportGenerator(this, jobArgument).generateReports();
            } catch (const common::ApplicationError &err) {
                BW_ERROR_ERR("Error when executing job '%s': %s", currentJob.c_str(), err.what());
            }
        }

        if (jobsExecuted == 0)
            BW_ERROR_ERR("Invalid job: '%s'", jobName.c_str());
    }

    if (m_upload)
        uploadReports();
}

} // end namespace reportgen
} // end namespace vetero
