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

#include <csignal>
#include <cassert>
#include <cerrno>
#include <algorithm>
#include <cstring>
#include <sys/wait.h>

#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>

#include "childprocesswatcher.h"

namespace vetero {
namespace daemon {

static void sigchild_handler(int signo)
{
    assert(signo== SIGCHLD);

    ChildProcessWatcher::instance()->handleZombies();
}

ChildProcessWatcher::ChildProcessWatcher()
{
    if (std::signal(SIGCHLD, sigchild_handler) == SIG_ERR)
        BW_ERROR_ERR("Unable to register handler for SIGCHLD: %s", std::strerror(errno));
}

ChildProcessWatcher::~ChildProcessWatcher()
{
    if (std::signal(SIGCHLD, SIG_IGN) == SIG_ERR)
        BW_ERROR_ERR("Unable to unregister handler for SIGCHLD: %s", std::strerror(errno));
}

void ChildProcessWatcher::addChild(pid_t pid)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);

    int err = sigprocmask(SIG_BLOCK, &set, NULL);
    if (err < 0)
        throw common::ApplicationError("Unable to block SIGCHLD");

    if (!wait(pid))
        m_children.insert(pid);

    err = sigprocmask(SIG_UNBLOCK, &set, NULL);
    if (err < 0)
        throw common::ApplicationError("Unable to unblock SIGCHLD");
}

bool ChildProcessWatcher::wait(pid_t pid)
{
    int status;
    pid_t rpid = waitpid(pid, &status, WNOHANG);
    if (rpid == static_cast<pid_t>(-1)) {
        BW_ERROR_ERR("Unable to call waitpid(): %s", std::strerror(errno));
        return false;
    } else if (rpid == pid) {
        int rc = WEXITSTATUS(status);
        if (rc == 0)
            BW_DEBUG_INFO("Child process %ld terminated.", long(rpid));
        else if (WIFSIGNALED(status)) {
            int signo = WTERMSIG(status);
            BW_ERROR_ERR("Child process %ld terminated with signal %d (%s)",
                         static_cast<long>(rpid), signo, strsignal(signo));
        } else
            BW_ERROR_ERR("Child process %ld terminated with exit status %d",
                         static_cast<long>(rpid), rc);

        // remove the PID from the list
        std::set<pid_t>::iterator pos = std::find(m_children.begin(), m_children.end(), rpid);
        if (pos != m_children.end())
            m_children.erase(pos);

        return true;
    } else {
        assert(rpid == 0);
        return false;
    }
}

void ChildProcessWatcher::handleZombies()
{
    bool childTerminated;

    do {
        childTerminated = false;
        for (std::set<pid_t>::const_iterator it = m_children.begin(); it != m_children.end(); ++it)
            if (wait(*it)) {
                childTerminated = true;
                break;
            }
    } while (childTerminated);
}

ChildProcessWatcher *ChildProcessWatcher::instance()
{
    static ChildProcessWatcher instance;
    return &instance;
}

} // end namespace daemon
} // end namespace vetero
