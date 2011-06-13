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
#ifndef VETEROD_H
#define VETEROD_H

#include <cstdio>
#include <memory>

#include "common/error.h"
#include "common/configuration.h"
#include "common/database.h"

/**
 * @file
 * @brief Contains the main class
 *
 * This files contains the main class of the application.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup daemon
 */

namespace vetero {
namespace daemon {

/* Veterod {{{ */

/**
 * @class Veterod
 * @brief Main class for the vetero daemon
 *
 * Main class of the application.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup daemon
 */
class Veterod {

    public:
        /**
         * @brief Constructor
         *
         * Creates a new Veterod instance.
         */
        Veterod();

        /**
         * @brief Destructor.
         */
        virtual ~Veterod();

        /**
         * @brief Parse the command line
         *
         * @param[in] argc the number of arguments
         * @param[in] argv the arguments
         * @return @c true if the application should be continued,
         *         @c false if the application should be quit
         * @exception common::ApplicationError if parsing the command line failed.
         */
        bool parseCommandLine(int argc, char *argv[])
        throw (common::ApplicationError);

        /**
         * @brief Installs the termination signal handlers
         *
         * @exception common::ApplicationError if registering the signal handlers failed.
         */
        void installSignalhandlers()
        throw (common::ApplicationError);

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
         * @brief Opens the database connection
         *
         * Opens the database as specified in the configuration file. If it doesn't exist, the
         * database will be created.
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
        void exec()
        throw (common::ApplicationError);

    protected:
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
         * @brief Setup the error logging for the application
         *
         * @exception common::ApplicationError if @p logfile cannot be opened for writing
         *            (if @p logfile is not a file but <tt>'syslog'</tt>, <tt>'stderr'</tt> or
         *            <tt>'stdout'</tt>, then no exception can be thrown)
         */
        void setupErrorLogging()
        throw (common::ApplicationError);

        /**
         * @brief Starts the display daemon
         *
         * Starts the display daemon if both Configuration::getDisplayName() and
         * Configuration::getDisplayConnection() return non-zero strings.
         *
         * @exception common::ApplicationError if the display daemon cannot be started.
         */
        void startDisplay()
        throw (common::ApplicationError);

        /**
         * @brief Updates the HTML reports
         *
         * Used the external program <tt>vetero-reportgen</tt>.
         *
         * @param[in] jobs the jobs specification as accepted by <tt>vetero-reportgen</tt>
         * @param[in] upload @c true if the files should also be uploaded, @c false otherwise.
         * @exception common::ApplicationError if the program cannot be started
         */
        void updateReports(const std::vector<std::string> &jobs, bool upload=false);

        /**
         * @brief Creates the pidfile
         */
        void createPidfile();

        /**
         * @brief Notifies the display daemon about new data.
         */
        void notifyDisplay();

    private:
        bool m_daemonize;
        FILE *m_logfile;
        std::string m_errorLogfile;
        std::string m_configfile;
        bool m_noConfigFatal;
        vetero::common::Sqlite3Database m_database;
        std::auto_ptr<vetero::common::Configuration> m_configuration;
};

/* }}} */

} // end namespace vetero
} // end namespace daemon

#endif // VETEROD_H
