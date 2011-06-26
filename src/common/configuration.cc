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
#include <cstdio>
#include <sstream>
#include <unistd.h>

#include <confuse.h>
#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>

#include <sys/types.h>
#include <pwd.h>

#include "configuration.h"
#include "config.h"

/**
 * @file
 * @brief Contains the configuration singleton
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup daemon
 */

namespace vetero {
namespace common {

/* Confuse error function {{{ */

// -------------------------------------------------------------------------------------------------
static void configuration_error_function(cfg_t *cfg, const char *fmt, va_list ap)
{
    bw::Errorlog *errorlog = bw::Errorlog::instance();
    errorlog->vlog(bw::Errorlog::LS_ERR, fmt, ap);
}

/* }}} */
/* Configuration {{{ */

// -------------------------------------------------------------------------------------------------
Configuration::Configuration(const std::string &preferredFilename)
    : m_serialDevice("/dev/ttyS0")
    , m_serialBaud(9600)
    , m_databasePath("vetero.db")
    , m_configurationRead(false)
{
    const std::string configfiles[] = {
            INSTALL_PREFIX "/etc/vetero.conf",
            "/etc/vetero.conf",
            std::string(getpwuid(getuid())->pw_dir) + "/.vetero.conf"
    };

    std::string filename;
    if (!preferredFilename.empty())
        filename = preferredFilename;
    else {
        for (size_t i = 0; i < sizeof(configfiles)/sizeof(configfiles[0]); i++)
            if (access(configfiles[i].c_str(), R_OK) == 0) {
                filename = configfiles[i];
                break;
            }

        if (filename.empty()) {
            m_error = "Unable to find a configuration file";
            return;
        }
    }

    read(filename);
}

// -------------------------------------------------------------------------------------------------
bool Configuration::configurationRead() const
{
    return m_configurationRead;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::error() const
{
    return m_error;
}

// -------------------------------------------------------------------------------------------------
void Configuration::read(const std::string &filename)
{
    char *serial_device = NULL, *database_path = NULL;
    char *report_directory = NULL, *report_upload_command = NULL;
    char *display_name = NULL, *display_connection = NULL;
    char *location_string = NULL;
    char *locale = NULL;
    int serial_baud = -1;

    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR(const_cast<char *>("serial_device"),         &serial_device),
        CFG_SIMPLE_INT(const_cast<char *>("serial_baud"),           &serial_baud),
        CFG_SIMPLE_STR(const_cast<char *>("database_path"),         &database_path),

        CFG_SIMPLE_STR(const_cast<char *>("report_directory"),      &report_directory),
        CFG_SIMPLE_STR(const_cast<char *>("report_upload_command"), &report_upload_command),
        CFG_SIMPLE_STR(const_cast<char *>("location_string"),       &location_string),

        CFG_SIMPLE_STR(const_cast<char *>("display_name"),          &display_name),
        CFG_SIMPLE_STR(const_cast<char *>("display_connection"),    &display_connection),

        CFG_SIMPLE_STR(const_cast<char *>("locale"),                &locale),
        CFG_END()
    };
    cfg_t *cfg;

    cfg = cfg_init(opts, 0);
    cfg_set_error_function(cfg, configuration_error_function);
    int err = cfg_parse(cfg, filename.c_str());
    if (err != CFG_SUCCESS) {
        if (err == CFG_FILE_ERROR)
            m_error = "Unable to open '" + filename + "' for reading";
        else if (err == CFG_PARSE_ERROR)
            m_error = "Error when parsing '" + filename + "'.";
        return;
    }

    cfg_free(cfg);

    if (serial_device) {
        m_serialDevice = serial_device;
        std::free(serial_device);
    }

    if (serial_baud > 0)
        m_serialBaud = serial_baud;

    if (database_path) {
        m_databasePath = database_path;
        std::free(database_path);
    }

    if (report_directory) {
        if (access(report_directory, W_OK) != 0)
            BW_ERROR_ERR("Directory '%s' not accessible writable. Disabling HTML reports.", report_directory);
        else {
            char *report_directory_real = realpath(report_directory, NULL);
            m_reportDirectory = report_directory_real;
            std::free(report_directory_real);
        }
        std::free(report_directory);
    }

    if (report_upload_command) {
        m_reportUploadCommand = report_upload_command;
        std::free(report_upload_command);
    }

    if (location_string) {
        m_locationString = location_string;
        std::free(location_string);
    }

    if (display_name) {
        m_displayName = display_name;
        std::free(display_name);
    }

    if (display_connection) {
        m_displayConnection = display_connection;
        std::free(display_connection);
    }

    if (locale) {
        m_locale = locale;
        std::free(locale);
    }

    m_configurationRead = true;

    BW_DEBUG_DBG("Parsing of configuration file '%s' finished: %s",
                 filename.c_str(), str().c_str());
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::serialDevice() const
{
    return m_serialDevice;
}

// -------------------------------------------------------------------------------------------------
int Configuration::serialBaud() const
{
    return m_serialBaud;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::databasePath() const
{
    return m_databasePath;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::reportDirectory() const
{
    return m_reportDirectory;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::reportUploadCommand() const
{
    return m_reportUploadCommand;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::locationString() const
{
    return m_locationString;
}

// -------------------------------------------------------------------------------------------------
void Configuration::setLocationString(const std::string &locationString)
{
    m_locationString = locationString;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::displayName() const
{
    return m_displayName;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::displayConnection() const
{
    return m_displayConnection;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::locale() const
{
    return m_locale;
}

// -------------------------------------------------------------------------------------------------
std::string Configuration::str() const
{
    std::stringstream ss;
    ss << std::boolalpha
       << "serialDevice="         << m_serialDevice           << ", "
       << "serialBaud="           << m_serialBaud             << ", "
       << "reportDirectory="      << m_reportDirectory        << ", "
       << "reportUploadCommand="  << m_reportUploadCommand    << ", "
       << "locationString="       << m_locationString         << ", "
       << "databasePath="         << m_databasePath           << ", "
       << "displayName="          << m_displayName            << ", "
       << "displayConnection="    << m_displayConnection      << ", "
       << "locale="               << m_locale;
    return ss.str();
}

/* }}} */

} // end namespace vetero
} // end namespace common

/* Output operator {{{ */
std::ostream &operator<<(std::ostream &os, vetero::common::Configuration &config)
{
    return os << config.str();
}
