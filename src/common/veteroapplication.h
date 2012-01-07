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
#ifndef VETERO_COMMON_VETEROAPPLICATION_H_
#define VETERO_COMMON_VETEROAPPLICATION_H_

#include <string>
#include <cstdio>

#include "error.h"

namespace vetero {
namespace common {

/* VeteroApplication {{{ */

/**
 * \class VeteroApplication
 * \brief Main class for all Vetero applications
 *
 * This abstract class shares code between the Vetero daemon, the vetero report generation tool and
 * the vetero display daemon.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class VeteroApplication
{
    public:
        /**
         * \brief C'tor
         *
         * \param[in] applicationName the name of the application. This string appears in syslog.
         */
        VeteroApplication(const std::string &applicationName);

        /**
         * \brief D'tor
         */
        virtual ~VeteroApplication();

    protected:
        /**
         * \brief Setup debug logging for the application
         *
         * \param[in] loglevel the minimum loglevel that gets logged. Valid values are \c "trace",
         *            \c "debug", \c "info" and \c "none".  Logging includes the given priority,
         *            i.e.  if \c debug is given, then messages of the severity \c debug and \c info
         *            are logged. Pass \c "none" for silence.
         * \param[in] filename if non-empty, log messages will not be printed to the console
         *            but redirected into \p filename.
         * \exception common::ApplicationError if the file cannot be created or if \p loglevel is invalid.
         */
        void setupDebugLogging(const std::string &loglevel, const std::string &filename);

        /**
         * \brief Setup the error logging for the application
         *
         * \param[in] errorLogfile the error log file, either <tt>"stderr"</tt>, <tt>"stdout"</tt>
         *            <tt>"syslog"</tt> or a real logfile.
         * \exception common::ApplicationError if \p logfile cannot be opened for writing
         *            (if \p logfile is not a file but <tt>'syslog'</tt>, <tt>'stderr'</tt> or
         *            <tt>'stdout'</tt>, then no exception can be thrown)
         */
        void setupErrorLogging(const std::string &errorLogfile);

    private:
        std::string m_applicationName;
        FILE *m_logfile;
};

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // VETERO_COMMON_VETEROAPPLICATION_H_
