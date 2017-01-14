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
#ifndef VETERO_VETEROD_VETEROD_H_
#define VETERO_VETEROD_VETEROD_H_

#include <memory>

#include "common/error.h"
#include "common/configuration.h"
#include "common/database.h"
#include "common/veteroapplication.h"

namespace vetero {
namespace daemon {

/* Veterod {{{ */

/**
 * \class Veterod
 * \brief Main class for the vetero daemon
 *
 * Main class of the application.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup daemon
 */
class Veterod : public common::VeteroApplication
{
    public:
        /**
         * \brief Constructor
         *
         * Creates a new Veterod instance.
         */
        Veterod();

        /**
         * \brief Parse the command line
         *
         * \param[in] argc the number of arguments
         * \param[in] argv the arguments
         * \return \c true if the application should be continued,
         *         \c false if the application should be quit
         * \exception common::ApplicationError if parsing the command line failed.
         */
        bool parseCommandLine(int argc, char *argv[]);

        /**
         * \brief Installs the termination signal handlers
         *
         * \exception common::ApplicationError if registering the signal handlers failed.
         */
        void installSignalhandlers();

        /**
         * \brief Reads the configuration file
         *
         * If a configuration file has been specified on the command line, read the
         * configuration file and throw an exception if the configuration file cannot be
         * read. If no configuration file has been specified, check if the default configuration
         * file exists and if yes, read it. Only if the file exists and cannot be parsed, an
         * \c common::ApplicationError is thrown.
         *
         * \exception common::ApplicationError if the configuration file cannot be read or parsed, see
         *            above for more information.
         */
        void readConfiguration();

        /**
         * \brief Opens the database connection
         *
         * Opens the database as specified in the configuration file. If it doesn't exist, the
         * database will be created.
         *
         * \exception common::ApplicationError if it's not possible to create the database.
         */
        void openDatabase();

        /**
         * \brief Main loop of the application
         *
         * This is the main part of the application.
         */
        void exec();

    protected:
        /**
         * \brief Starts the display daemon
         *
         * Starts the display daemon if both Configuration::getDisplayName() and
         * Configuration::getDisplayConnection() return non-zero strings.
         *
         * \exception common::ApplicationError if the display daemon cannot be started.
         */
        void startDisplay();

        /**
         * \brief Updates the HTML reports
         *
         * Used the external program <tt>vetero-reportgen</tt>.
         *
         * \param[in] jobs the jobs specification as accepted by <tt>vetero-reportgen</tt>
         * \param[in] upload \c true if the files should also be uploaded, \c false otherwise.
         * \exception common::ApplicationError if the program cannot be started
         */
        void updateReports(const std::vector<std::string> &jobs, bool upload=false);

        /**
         * \brief Creates the pidfile
         */
        void createPidfile();

        /**
         * \brief Notifies the display daemon about new data.
         */
        void notifyDisplay();

        /**
         * \brief Checks if we have a pressure sensor configured
         *
         * \retval true if a pressure sensor is attached and configured
         * \retval false if no pressure sensor is attached and configured
         */
        bool havePressureSensor() const;

        /**
         * \brief Sets some weather values in the process environment
         *
         * This environment can be used the vetero postscript.
         */
        void updateEnvironment(const vetero::common::UsbWde1Dataset &dataset,
                int rainValue);

        /**
         * \brief Executes the postscript, if there's any
         */
        void runPostscript(const vetero::common::UsbWde1Dataset &dataset,
                int rainValue);
    private:
        bool m_daemonize;
        std::string m_errorLogfile;
        std::string m_configfile;
        bool m_noConfigFatal;
        vetero::common::Sqlite3Database m_database;
        std::auto_ptr<vetero::common::Configuration> m_configuration;
};

/* }}} */

} // end namespace vetero
} // end namespace daemon

#endif // VETERO_VETEROD_VETEROD_H_
