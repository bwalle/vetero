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
#ifndef VETERO_COMMON_CONFIGURATION_H_
#define VETERO_COMMON_CONFIGURATION_H_

#include <cstdio>

#include <common/error.h>

namespace vetero {
namespace common {

/* Configuration {{{ */

/**
 * \class Configuration
 * \brief Singleton configuration object for the daemon
 *
 * This class is implemented as singleton and represents the configuration of the application.
 * The configuration need to be read by parsing the configuration file with Configuration::read().
 * Otherwise the default values are used.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class Configuration
{
    public:
        /**
         * \brief Constructor
         *
         * Creates a new Veterod instance. Parses the given configuration file.
         *
         * \param[in] filename the name of the configuration file that should be read. If no file name is
         *            specified, the default search is used.
         */
        Configuration(const std::string &filename="");

        /**
         * \brief Destructor.
         */
        virtual ~Configuration() {}

    public:
        /**
         * \brief Checks if the configuration was read in the C'tor
         *
         * \return \c true if the configuration was read, \c false otherwise
         */
        bool configurationRead() const;

        /**
         * \brief Returns the error string
         *
         * If configurationRead() returns \c false, this function returns an error string.
         *
         * \return the human-readable error
         */
        std::string error() const;

        /** \name Serial device access */                                                  /* {{{ */
        /** \{ */

        /**
         * \brief Returns the serial device
         *
         * The default device is \c "/dev/ttyS0".
         *
         * \return the path to the serial device, e.g. \c /dev/ttyS0
         */
        std::string serialDevice() const;

        /**
         * \brief Returns the baudrate
         *
         * The default baudrate is 9600.
         *
         * \return the baudrate
         */
        int serialBaud() const;

        /** \} */                                                                          /* }}} */

        /** \name Pressure sensor */                                                       /* {{{ */
        /** \{ */

        /**
         * \brief Returns the bus number of the I2C device where the bmp085 sensor is connected to
         *
         * The default device is \c "/dev/ttyS0".
         *
         * \return the bus number (starting with 0 as the first bus) or a negative value
         *         if the value is unset (meaning that there's no bmp085 sensor)
         */
        int pressureSensorI2cBus() const;

        /**
         * \brief Returns the height (in metres) that is used to calculate the air pressure
         *
         * A negative valueThe default baudrate is 9600.
         *
         * \return the height (which can be 0) or a negative value if there is no height configured.
         *         In that case, a zero height is assumed which means that the pressure of the
         *         sensor is taken without any correction.
         */
        int pressureHeight() const;

        /** \} */                                                                          /* }}} */

        /** \name Database */                                                              /* {{{ */
        /** \{ */

        /**
         * \brief Returns the path to the database
         *
         * The default path is \c "vetero.db" which depends on the current working directory of the
         * application.
         *
         * \return the database path, either absolute or relative.
         */
        std::string databasePath() const;

        /** \} */                                                                          /* }}} */

        /** \name Report generation */                                                     /* {{{ */
        /** \{ */

        /**
         * \brief Returns the dark color of the title bar in the HTML reports
         *
         * \return the command
         */
        std::string reportTitleColor1() const;

        /**
         * \brief Returns the bright color of the title bar in the HTML reports
         *
         * \return the command
         */
        std::string reportTitleColor2() const;

        /**
         * \brief Returns the HTML reports are put into
         *
         * \return the full path to the directory
         */
        std::string reportDirectory() const;

        /**
         * \brief Returns the command that uploads the HTML reports
         *
         * \return the command
         */
        std::string reportUploadCommand() const;

        /**
         * \brief Returns the location string
         *
         * This string is shown in the HTML pages.
         *
         * \return the location string
         */
        std::string locationString() const;

        /**
         * \brief Returns the locale
         *
         * \return the locale like <tt>"de_DE.utf-8"</tt>.
         */
        std::string locale() const;

        /** \} */                                                                          /* }}} */

        /** \name LCD */                                                                   /* {{{ */
        /** \{ */

        /**
         * \brief Returns the display name for serdisplib
         *
         * Used to start the display program (if available).
         *
         * \return the display name, e.g. <tt>'CTINCLUD'</tt>.
         */
        std::string displayName() const;

        /**
         * \brief Returns the display connection for serdisplib.
         *
         * Used to start the display program (if available).
         *
         * \return the display connection, e.g. <tt>'USB:7c0/1501'</tt>.
         */
        std::string displayConnection() const;

        /** \} */                                                                          /* }}} */

        /**
         * \brief Converts the object to a human-readable string
         *
         * Mainly for debug output
         *
         * \return the string
         */
        std::string str() const;

    protected:
        /**
         * \brief Parses the given configuration files
         *
         * Reads the configuration file \p filename.
         *
         * \param[in] filename the name of the configuration file that should be read
         * \exception ApplicationError if the file cannot be parsed.
         */
        void read(const std::string &filename);

    private:
        std::string m_serialDevice;
        int         m_serialBaud;
        int         m_pressureSensorI2cBus;
        int         m_pressureHeight;
        std::string m_reportTitleColor1;
        std::string m_reportTitleColor2;
        std::string m_reportDirectory;
        std::string m_reportUploadCommand;
        std::string m_locationString;
        std::string m_databasePath;
        std::string m_displayName;
        std::string m_displayConnection;
        bool        m_configurationRead;
        std::string m_locale;
        std::string m_error;
};

/* }}} */

} // end namespace common
} // end namespace vetero

/* Output operator {{{ */

/**
 * \brief Prints a configuration to the given output stream
 *
 * Calls Configuration::str() internally.
 *
 * \param[in] os the output stream
 * \param[in] config the configuration to dump
 * \return \p os
 */
std::ostream &operator<<(std::ostream &os, vetero::common::Configuration &config);

/* }}} */

#endif // VETERO_COMMON_CONFIGURATION_H_
