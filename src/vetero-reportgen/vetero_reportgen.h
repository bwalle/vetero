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
#ifndef VETERO_REPORTGEN_VETERO_REPORTGEN_H_
#define VETERO_REPORTGEN_VETERO_REPORTGEN_H_

#include <string>
#include <memory>

#include "common/error.h"
#include "common/configuration.h"
#include "common/database.h"

namespace vetero {
namespace reportgen {

/**
 * @class VeteroReportgen
 * @brief Report generation class
 *
 * This is the main class for the report generation tool.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */
class VeteroReportgen {

    public:
        /**
         * @brief Enumeration type that describes the ports that can be generated
         */
        enum Report {
            REPORT_CURRENT,             /**< the current weather including min/max of the day (i.e. basically
                                             the same which is displayed on the LCD display */
            REPORT_DAY,                 /**< the statistics for one day */
            REPORT_MONTH,               /**< the statistics for one month */
            REPORT_YEAR                 /**< the statistics for one year */
        };

    public:
        /**
         * @brief Constructor
         */
        VeteroReportgen();

    public:
        /**
         * @brief Returns a reference to the DB access object.
         *
         * @return the DB access object.
         */
        vetero::common::Sqlite3Database &database();

        /**
         * @brief Returns a reference to the configuration object
         *
         * @return the reference
         */
        vetero::common::Configuration &configuration();

        /**
         * @brief Returns a reference to the configuration object
         *
         * @return the const reference
         */
        const vetero::common::Configuration &configuration() const;

        /**
         * @brief Reads the configuration file
         *
         * If a configuration file has been specified on the command line, read the
         * configuration file and throw an exception if the configuration file cannot be
         * read. If no configuration file has been specified, check if the default configuration
         * file exists and if yes, read it. Only if the file exists and cannot be parsed, an
         * @c common::ApplicationError is thrown.
         *
         * @exception common::ApplicationError if the configuration file cannot be read or parsed, see
         *            above for more information.
         */
        void readConfiguration()
        throw (common::ApplicationError);

        /**
         * @brief Parse the command line
         *
         * @param[in] argc the number of arguments
         * @param[in] argv the arguments
         * @return @c true if the application should be continued,
         *         @c false if the application should be quit
         * @exception ApplicationError if parsing the command line failed.
         */
        bool parseCommandLine(int argc, char *argv[])
        throw (common::ApplicationError);

        /**
         * @brief Opens the database connection
         *
         * Opens the database as specified on the command line. If it doesn't exist, the database will be created.
         *
         * @exception common::ApplicationError if it's not possible to create the database.
         */
        void openDatabase()
        throw (common::ApplicationError);

        /**
         * @brief Main loop of the application
         *
         * This is the main part of the application.
         */
        void exec();

    protected:
        /**
         * @brief Setup the error logging for the application
         *
         * @param[in] logfile the logfile to be used for errors. The special values
         *            <tt>'stdout'</tt>, <tt>'stderr'</tt> for the standard I/O streams and the
         *            special value <tt>'syslog'</tt> (for syslog()) is recognized.
         * @exception ApplicationError if @p logfile cannot be opened for writing
         *            (if @p logfile is not a file but <tt>'syslog'</tt>, <tt>'stderr'</tt> or
         *            <tt>'stdout'</tt>, then no exception can be thrown)
         */
        void setupErrorLogging(const std::string &logfile)
        throw (common::ApplicationError);

        /**
         * @brief Setup debug logging for the application
         *
         * @param[in] loglevel the minimum loglevel that gets logged. Valid values are @c "trace",
         *            @c "debug", @c "info" and @c "none".  Logging includes the given priority,
         *            i.e.  if @c debug is given, then messages of the severity @c debug and @c info
         *            are logged. Pass @c "none" for silence.
         * @param[in] filename if non-empty, log messages will not be printed to the console
         *            but redirected into @p filename.
         * @exception common::ApplicationError if the file cannot be created or if @p loglevel is invalid.
         */
        void setupDebugLogging(const std::string &loglevel, const std::string &filename)
        throw (common::ApplicationError);

        /**
         * @brief Performs the upload of reports
         */
        void uploadReports();

    private:
        vetero::common::Sqlite3Database m_database;
        std::vector<std::string> m_jobs;
        FILE *m_logfile;

        std::string m_configfile;
        bool m_noConfigFatal;
        std::auto_ptr<vetero::common::Configuration> m_configuration;

        bool m_upload;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_VETERO_REPORTGEN_H_
