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

#include <libbw/compiler.h>

#include "common/error.h"

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
 * Don't need a static buffer but still having the advantage of the simplicity
 * of printf().
 *
 * This function uses the default locale and is a convenience function that
 * calls str_printf_l() with NULL as second parameter.
 *
 * @param[in] format the format string
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_printf(const char *format, ...)
    COMPILER_PRINTF_FORMAT(1, 2);

/**
 * @brief printf_l() implementation that allocates the result string
 *        automatically 
 *
 * Don't need a static buffer but still having the advantage of the simplicity
 * of printf(). The function is even available on systems without printf_l()
 * implementation like Linux.
 *
 * @param[in] format the ofrmat string
 * @param[in] locale a string that describes the locale to be used instead of
 *            the global locale. Unlike the system functions, @c NULL means
 *            to use the global locale. If you want the traditional C locale
 *            to be used, pass <tt>"C"</tt> for @p locale. The empty string
 *            <tt>""</tt> means that the system should find out the locale
 *            from the environemnt variables <tt>LANG</tt> and <tt>LC_</tt>.
 *            See locale(3), xlocale(3) and setlocale(3) manual pages.
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_printf_l(const char *format, const char *locale,  ...)
    COMPILER_PRINTF_FORMAT(1, 3);

/**
 * @brief printf() implementation that allocates the result string automatically
 *        (va_list variant)
 *
 * Don't need a static buffer but still having the advantage of the simplicity
 * of printf().
 *
 * This function uses the default locale and is a convenience function that
 * calls str_vprintf_l() with NULL as second parameter.
 *
 * @param[in] format the format string
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_vprintf(const char *format, va_list ap)
    COMPILER_PRINTF_FORMAT(1, 0);

/**
 * @brief printf() implementation that allocates the result string automatically
 *        (va_list variant)
 *
 * Don't need a static buffer but still having the advantage of the simplicity
 * of printf().
 *
 * @param[in] format the format string
 * @param[in] locale a string that describes the locale to be used instead of
 *            the global locale. Unlike the system functions, @c NULL means
 *            to use the global locale. If you want the traditional C locale
 *            to be used, pass <tt>"C"</tt> for @p locale. The empty string
 *            <tt>""</tt> means that the system should find out the locale
 *            from the environemnt variables <tt>LANG</tt> and <tt>LC_</tt>.
 *            See locale(3), xlocale(3) and setlocale(3) manual pages.
 * @return the formated string
 * @exception std::bad_alloc if it was not possible to allocate memory
 */
std::string str_vprintf_l(const char *format, const char *locale, va_list ap)
    COMPILER_PRINTF_FORMAT(1, 0);

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
