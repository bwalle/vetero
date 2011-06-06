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
#ifndef VETERO_DISPLAYD_H
#define VETERO_DISPLAYD_H

#include <memory>

#include <common/database.h>
#include <common/error.h>
#include <common/configuration.h>

#include "serdisplibdisplay.h"

/**
 * @file
 * @brief Contains the main class
 *
 * This files contains the main class of the display application.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */

namespace vetero {
namespace display {

/* VeteroDisplayd {{{ */

/**
 * @class VeteroDisplayd
 * @brief Main class for the vetero display daemon
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class VeteroDisplayd {

    public:
        /**
         * @brief Constructor
         *
         * Creates a new VeteroDisplayd instance.
         */
        VeteroDisplayd();

        /**
         * @brief Destructor.
         */
        virtual ~VeteroDisplayd();

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
         * Opens the database as specified on the command line. If it doesn't exist, the database will be created.
         *
         * @exception common::ApplicationError if it's not possible to create the database.
         */
        void openDatabase()
        throw (common::ApplicationError);

        /**
         * @brief Opens the display connection
         *
         * Opens the display.
         *
         * @throw common::ApplicationError if some error occurs.
         */
        void openDisplay()
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
         * @brief Updates the contents of the display
         *
         * @param[in] weather the weather data to display
         * @exception ApplicationError if an error occurred
         */
        void updateDisplay(const common::CurrentWeather &weather)
        throw (common::ApplicationError);

    private:
        std::string m_configfile;
        bool m_noConfigFatal;
        vetero::common::Sqlite3Database m_database;
        std::auto_ptr<vetero::common::Configuration> m_configuration;
        SerdisplibConnection *m_serdispConnection;
        SerdisplibTextDisplay *m_display;
};

/* }}} */

} // end namespace vetero
} // end namespace display

#endif // VETERO_DISPLAYD_H
