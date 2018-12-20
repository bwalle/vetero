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
#include <locale.h>
#include <sys/stat.h>

#include <zlib.h>

#include <libbw/log/errorlog.h>
#include <libbw/stringutil.h>

#include "utils.h"

namespace vetero {
namespace common {

std::string dashDecimalValue(const std::string &locale, int dashesBefore, int dashesAfter)
{
    locale_t oldlocale = NULL;
    locale_t clocale = NULL;

    if (!locale.empty() && dashesAfter > 0) {
        clocale = newlocale(LC_NUMERIC_MASK, locale.c_str(), NULL);
        if (clocale)
            oldlocale = uselocale(clocale);
        else {
            oldlocale = uselocale(NULL);
            BW_ERROR_WARNING("Unable to set new locale (%s) with uselocale(): %s",
                             locale.c_str(), std::strerror(errno));
        }
    }

    std::string ret(dashesBefore, '-');
    if (dashesAfter > 0) {
        ret += localeconv()->decimal_point;
        ret += std::string(dashesAfter, '-');
    }

    // restore locale
    if (oldlocale)
        uselocale(oldlocale);
    if (clocale)
        freelocale(clocale);

    return ret;
}

std::string str_printf(const char *format, ...)
{
    std::va_list ap;

    va_start(ap, format);
    std::string ret = str_vprintf_l(format, NULL, ap);
    va_end(ap);

    return ret;
}

std::string str_printf_l(const char *format, const char *locale, ...)
{
    std::va_list ap;

    va_start(ap, locale);
    std::string ret = str_vprintf_l(format, locale, ap);
    va_end(ap);

    return ret;

}

std::string str_vprintf(const char *format, va_list ap)
{
    return str_vprintf_l(format, NULL, ap);
}

std::string str_vprintf_l(const char *format, const char *locale, va_list ap)
{
    // uselocale() is the thread-safe variant of setlocale()
    // The _l() functions of the printf() family are not available on Linux.

    locale_t oldlocale = NULL;
    locale_t clocale = NULL;

    if (locale) {
        clocale = newlocale(LC_NUMERIC_MASK, locale, NULL);
        if (clocale)
            oldlocale = uselocale(clocale);
        else {
            oldlocale = uselocale(NULL);
            BW_ERROR_WARNING("Unable to set new locale (%s) with uselocale(): %s",
                             locale, std::strerror(errno));
        }
    }

    char *ret_allocated;
    vasprintf(&ret_allocated, format, ap);

    // restore locale
    if (oldlocale)
        uselocale(oldlocale);
    if (clocale)
        freelocale(clocale);

    if (!ret_allocated)
        throw std::bad_alloc();

    std::string ret(ret_allocated);
    std::free(ret_allocated);

    return ret;
}

pid_t start_background(const std::string &process, const std::vector<std::string> &args)
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

    throw SystemError("Unable to fork()", errno);
}

void compress_file(const std::string &filename)
{
    // use the C file I/O since it's equivalent with zlib I/O

    std::FILE *fp = std::fopen(filename.c_str(), "r");
    if (!fp)
        throw common::SystemError("Unable to open '" + filename + "' for reading", errno);

    // get the size of the file
    int ret = std::fseek(fp, 0, SEEK_END);
    if (ret < 0) {
        std::fclose(fp);
        throw common::SystemError("Unable to seek '" + filename + "' to the end", errno);
    }

    off_t size = ftello(fp);
    std::rewind(fp);

    char *buffer = new char[size];
    ret = std::fread(buffer, 1, size, fp);
    std::fclose(fp);
    if (ret != size) {
        delete[] buffer;
        throw common::SystemError("Unable to read " + bw::str(size) + " bytes from " + filename, errno);
    }

    gzFile outfp = gzopen(filename.c_str(), "w");
    if (!outfp)
        throw common::ApplicationError("Unable to open '" + filename + "' for writing: ");

    ret = gzwrite(outfp, buffer, size);
    delete[] buffer;
    gzclose(outfp);
    if (ret != size)
        throw common::ApplicationError("Unable to write to '" + filename + "'");
}

std::string realpath(const std::string &filename)
{
    char *resolved = ::realpath(filename.c_str(), NULL);
    if (!resolved)
        throw common::SystemError("Unable to resolve '" + filename + "'", errno);

    std::string ret(resolved);
    std::free(resolved);

    return ret;
}

} // end namespace common
} // end namespace vetero
