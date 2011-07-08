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

#ifndef LOCKFILE_H_
#define LOCKFILE_H_

#include <string>
#include <sstream>

namespace vetero {
namespace common {

/**
 * @brief Locking resources with lock files
 *
 * This class can lock a ressource shared between multiple processes by using a lock file.
 * The lock file must already exist and can also be a directory.
 *
 * The lock is released when the object is destroyed. The implementation is done with
 * the flock() system call, so read the manpage for details.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */
class LockFile
{
    public:
        /**
         * @brief Constructor
         *
         * This doesn't create the lock, it just opens a file descriptor.
         *
         * @param[in] filename
         */
        LockFile(const std::string &filename);

        /**
         * @brief Destructor
         *
         * Releases the lock.
         */
        virtual ~LockFile();

    public:
        /**
         * @brief Performs a shared lock
         *
         * This function blocks until the lock is available.
         *
         * @return @c true on success, @c false on failure.
         */
        bool lockShared();

        /**
         * @brief Performs an exclusive lock
         *
         * This function blocks until the lock is available.
         *
         * @return @c true on success, @c false on failure.
         */
        bool lockExclusive();

        /**
         * @brief Unlocks the file
         *
         * @return @c true on success, @c false on failure.
         */
        bool unlock();

        /**
         * @brief Returns a human-readable error message whenever functions fail.
         *
         * @return the error text
         */
        std::string error() const;

    private:
        int m_fd;
        std::string m_filename;
        std::stringstream m_error;
};

} // end namespace common
} // end namespace vetero

#endif /* LOCKFILE_H_ */
