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

#ifndef VETERO_VETEROD_CHILDPROCESSWATCHER_H_
#define VETERO_VETEROD_CHILDPROCESSWATCHER_H_

#include <set>
#include <sys/types.h>

#include "common/error.h"

namespace vetero {
namespace daemon {

/**
 * \brief Singleton to monitor child processes on Unix
 *
 * The special thing about this class is that it only calls waitpid() on the processes
 * that it maintains in a list. This makes it possible to deal with other processes synchronously.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup daemon
 */
class ChildProcessWatcher {

    private:
        /**
         * \brief Constructor
         *
         * Use ChildProcessWatcher::instance(). Registers the child handler.
         */
        ChildProcessWatcher();

        /**
         * \brief Destructor
         *
         * Unregisters the signal handler for SIGCHLD.
         */
        virtual ~ChildProcessWatcher();

    public:
        /**
         * \brief Singleton accessor
         *
         * \return the only instance of ChildProcessWatcher.
         */
        static ChildProcessWatcher *instance();

        /**
         * \brief Adds a child to monitor
         *
         * Handles also the case that SIGCHLD was already called for that process.
         *
         * \param[in] pid the PID of the child process
         */
        void addChild(pid_t pid);

        /**
         * \brief Calls waitpid() on all monitored chilren.
         *
         * Prints an error in the error log if the process exited with a status unequal to zero.
         */
        void handleZombies();

    protected:
        /**
         * \brief Calls waitpid() for \p pid
         *
         * This function does the error logging and removes the pid from the list.
         *
         * \param[in] pid the pid to watch for
         * \return \c true if \p pid has terminated and the zombie was deleted, \c false otherwise
         */
        bool wait(pid_t pid);

    private:
        std::set<pid_t> m_children;
};

} // end namespace daemon
} // end namespace vetero

#endif // VETERO_VETEROD_CHILDPROCESSWATCHER_H_
