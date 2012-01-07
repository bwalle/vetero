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

#include <cstring>
#include <cerrno>

#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

#include "lockfile.h"

namespace vetero {
namespace common {

LockFile::LockFile(const std::string &filename)
    : m_fd(-1)
    , m_filename(filename)
{
    m_fd = open(filename.c_str(), O_RDONLY);
    if (m_fd < 0)
        m_error << "Unable to open '" << filename << "': " << std::strerror(errno);
}

LockFile::~LockFile()
{
    if (!unlock())
        BW_ERROR_ERR("Unable to unlock '%s': %s", m_filename.c_str(), error().c_str());
}

bool LockFile::lockShared()
{
    if (m_fd < 0)
        return false;

    BW_DEBUG_DBG("Obtaining shared lock for '%s'", m_filename.c_str());

    int err = flock(m_fd, LOCK_SH);
    if (err < 0) {
        m_error << "Unable to create shared lock on '" << m_filename << "': " << std::strerror(errno);
        return false;
    }

    return true;
}

bool LockFile::lockExclusive()
{
    if (m_fd < 0)
        return false;

    BW_DEBUG_DBG("Obtaining exclusive lock for '%s'", m_filename.c_str());

    int err = flock(m_fd, LOCK_EX);
    if (err < 0) {
        m_error << "Unable to create exclusive lock on '" << m_filename << "': " << std::strerror(errno);
        return false;
    }

    return true;
}

bool LockFile::unlock()
{
    if (m_fd < 0)
        return false;

    BW_DEBUG_DBG("Unlocking '%s'", m_filename.c_str());

    int err = flock(m_fd, LOCK_UN);
    if (err < 0) {
        m_error << "Unable to create exclusive lock on '" << m_filename << "': " << std::strerror(errno);
        return false;
    }

    return true;
}

std::string LockFile::error() const
{
    return m_error.str();
}

} // end namespace common
} // end namespace vetero
