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
#ifndef CHARSET_H
#define CHARSET_H

#include <stdexcept>
#include <string>

namespace vetero {
namespace display {

/* CharsetError {{{ */

/**
 * @class CharsetError
 * @brief Exception class used in native2latin1().
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class CharsetError : public std::runtime_error {

    public:
        /**
         * @brief Constructor
         *
         * Creates a new CharsetError.
         *
         * @param[in] string the error string
         */
        CharsetError(const std::string &string)
            : std::runtime_error(string) {}

        /**
         * @brief Destructor
         */
        virtual ~CharsetError()
        throw () {}
};

/* }}} */

/**
 * @brief Querys the system's default character set
 *
 * Uses the UNIX locale mechanism (<tt>LC_CTYPE</tt> in particular, see locale(7)) to get
 * the character set.
 *
 * @warning As this function uses setlocale(3), it's not thread-safe. The function restores the old
 *          locale state, but there may be race conditions. However, the result is cached so if you
 *          call queryNativeCharset() once in the initialization of the program before starting
 *          other threads you're on the safe side.
 *
 * @return a string like <tt>"iso-8859-1"</tt> or <tt>"utf-8"</tt> suitable for iconv(3).
 * @exception CharsetError if it's not possible to get the character set of the system
 * @ingroup display
 */
std::string queryNativeCharset();

/**
 * @brief Converts the native string to iso-8859-1
 *
 * The character set of @p native is determined by executing queryNativeCharset().
 * The return string will encoded in iso-8859-1. Characters that cannot be encoded in
 * iso-8859-1 are encoded as <tt>'?'</tt>.
 *
 * This function is equivalent to toLatin1() with @p charset set to <tt>""</tt>.
 *
 * @param[in] native a string encoded in the system's native character set
 * @return the string encoded in iso-8859-1
 * @exception CharsetError if @p native contains invalid characters
 * @ingroup display
 * @sa toLatin1()
 */
std::string native2latin1(const std::string &native)
throw (CharsetError);

/**
 * @brief Converts the given string from an arbitrary charset to Latin1
 *
 * Converts the string @p stringToConvert which must be in charset @p charset
 * to iso-8859-1.
 *
 * @param[in] stringToConvert a string encoded in @p charset
 * @param[in] charset the character set as iconv understands. The system command <tt>iconv -l</tt>
 *            gives a list of available charsets. If @p charset is the empty string, the charset as
 *            returned by queryNativeCharset() is used.
 * @return the string encoded in iso-8859-1
 * @exception CharsetError if @p stringToConvert contains invalid characters or if @p charset
 *            doesn't name a valid character set.
 * @ingroup display
 * @sa native2latin1()
 */
std::string toLatin1(const std::string &stringToConvert, const std::string &charset)
throw (CharsetError);

/**
 * @brief Converts the iso-8859-1 encoded input to the HD44780 display charset
 *
 * @param[in] latin1 the iso-8859-1 encoded input string
 * @return the HD44780-encoded output
 * @ingroup display
 */
std::string latin12hd44780(const std::string &latin1);

} // end namespace vetero
} // end namespace display

#endif // CHARSET_H
