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
#include <stdexcept>
#include <clocale>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <langinfo.h>
#include <iconv.h>

#include "hd44780-charmap.h"
#include "charset.h"

namespace vetero {
namespace display {

// --------------------------------------------------------------------------------------------------
std::string queryNativeCharset()
{
    static std::string charset;

    if (charset.empty()) {
        std::string old_locale = std::setlocale(LC_CTYPE, NULL);
        std::setlocale(LC_CTYPE, "");
        charset = nl_langinfo(CODESET);
        std::setlocale(LC_CTYPE, old_locale.c_str());
    }

    return charset;
}

// --------------------------------------------------------------------------------------------------
std::string native2latin1(const std::string &native)
throw (CharsetError)
{
    return toLatin1(native, "");
}

// -------------------------------------------------------------------------------------------------
std::string toLatin1(const std::string &stringToConvert, const std::string &charset)
    throw (CharsetError)
{
    std::string sourceCharset = charset;
    
    if (sourceCharset.empty())
        sourceCharset = queryNativeCharset();

    iconv_t iconv_handle = iconv_open("iso-8859-1", sourceCharset.c_str());
    if (iconv_handle == reinterpret_cast<iconv_t>(-1))
        throw CharsetError("Unable to obtain iconv handle for '"+ sourceCharset +"' -> 'iso-8859-1'");

    // the result *must* be smaller or equal because we know that it's latin1.
    char *resultbuffer = new char[stringToConvert.size()+1];

    // iconv doesn't write to the buffer, it just modifies the input pointer
    char *input = const_cast<char *>(stringToConvert.c_str());
    size_t inbytesleft = stringToConvert.size();
    char *output = resultbuffer;
    size_t outbytesleft = stringToConvert.size();

    int ret = iconv(iconv_handle, &input, &inbytesleft, &output, &outbytesleft);
    iconv_close(iconv_handle);

    if (ret < 0) {
        delete[] resultbuffer;
        throw CharsetError("Error when converting '"+ stringToConvert +"' ("+ sourceCharset +") to latin1: " +
                           std::string(std::strerror(errno)) );
    }

    resultbuffer[stringToConvert.size() - outbytesleft] = '\0';
    std::string result(resultbuffer);

    delete[] resultbuffer;

    return result;
}

// -------------------------------------------------------------------------------------------------
std::string latin12hd44780(const std::string &latin1)
{
    std::string result;

    for (size_t i = 0; i < latin1.size(); ++i)
        result += char(HD44780_charmap[ static_cast<unsigned char>(latin1[i]) ]);

    return result;
}

} // end namespace vetero
} // end namespace display
