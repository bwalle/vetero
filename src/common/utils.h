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
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <stdexcept>
#include <string>
#include <cstdarg>

#include <common/error.h>

/**
 * @file
 * @brief Contains utility functions used in Vetero.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */

namespace vetero {
namespace common {

/**
 * @brief printf() implementation that allocates the result string automatically
 *
 * Don't need a static buffer but still having the advantage of the simplicity of printf().
 *
 * @param[in] format the format string
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_printf(const char *format, ...);

/**
 * @brief printf() implementation that allocates the result string automatically (va_list variant)
 *
 * Don't need a static buffer but still having the advantage of the simplicity of printf().
 *
 * @param[in] format the format string
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_vprintf(const char *format, va_list ap);

/**
 * @brief Starts a process in the background
 *
 * @param[in] process the name of the process (can be in <tt>PATH</tt>)
 * @param[in] args the process' argument
 * @return the PID of the started process (the caller has to handle <tt>SIGCHLD</tt>)
 * @exception common::ApplicationError if the process cannot be started
 */
pid_t start_background(const std::string &process, const std::vector<std::string> &args)
throw (common::ApplicationError);

} // end namespace common
} // end namespace vetero

#endif // UTILS_H
