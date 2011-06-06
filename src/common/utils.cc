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
#define _GNU_SOURCE 1

#include <cstdio>
#include <stdexcept>
#include <string>
#include <cstdarg>
#include <cmath>
#include <cstdlib>
#include <cerrno>

#include "utils.h"

namespace vetero {
namespace common {

// -------------------------------------------------------------------------------------------------
std::string str_printf(const char *format, ...)
{
    std::va_list ap;

    va_start(ap, format);
    std::string ret = str_vprintf(format, ap);
    va_end(ap);

    return ret;
}

// -------------------------------------------------------------------------------------------------
std::string str_vprintf(const char *format, va_list ap)
{
    char *ret;

    vasprintf(&ret, format, ap);
    if (!ret)
        throw std::bad_alloc();

    return std::string(ret);
}

// -------------------------------------------------------------------------------------------------
pid_t start_background(const std::string &process, const std::vector<std::string> &args)
    throw (common::ApplicationError)
{
    pid_t childpid = fork();
    if (childpid == 0) {
        const char *argVector[args.size() + 2];

        argVector[0] = process.c_str();
        for (size_t i = 1; i <= args.size(); ++i)
            argVector[i] = args[i-1].c_str();
        argVector[args.size()+1] = NULL;

        int ret = execvp(process.c_str(), const_cast<char **>(argVector));
        if (ret < 0)
            std::exit(-1);
    } else if (childpid > 0)
        return childpid;
    else
        throw SystemError("Unable to fork()", errno);
}

} // end namespace common
} // end namespace vetero
