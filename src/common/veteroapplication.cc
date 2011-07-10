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

#include <cerrno>

#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>

#include "veteroapplication.h"

namespace vetero {
namespace common {

/* VeteroApplication {{{ */

// -------------------------------------------------------------------------------------------------
VeteroApplication::VeteroApplication(const std::string &applicationName)
    : m_applicationName(applicationName)
    , m_logfile(NULL)
{}

// -------------------------------------------------------------------------------------------------
VeteroApplication::~VeteroApplication()
{
    if (m_logfile) {
        std::fclose(m_logfile);
        m_logfile = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
void VeteroApplication::setupDebugLogging(const std::string &levelstring, const std::string &filename)
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
void VeteroApplication::setupErrorLogging(const std::string &errorLogfile)
    throw (common::ApplicationError)
{
    if (errorLogfile == "syslog")
        bw::Errorlog::configure(bw::Errorlog::LM_SYSLOG, "veterod");
    else {
        bool success = bw::Errorlog::configure(bw::Errorlog::LM_FILE, errorLogfile.c_str());
        if (!success)
            throw common::ApplicationError("Unable to setup error logging for '" + errorLogfile + "'");
    }
}


/* }}} */

} // end namespace common
} // end namespace vetero
