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
#include <stdio.h>

#include <iostream>
#include <algorithm>
#include <cstdarg>

#include <libbw/stringutil.h>

#include "common/utils.h"
#include "serdisplibdisplay.h"
#include "charset.h"
#include "font_6x8.h"
#include "font_6x8_bold.h"

namespace vetero {
namespace display {

/* SerdisplibConnection {{{ */

SerdisplibConnection::SerdisplibConnection(const std::string &sdcdev)
    : m_connection(NULL)
{
    m_connection = SDCONN_open(sdcdev.c_str());
    if (!m_connection)
        throw DisplayError("Unable to create a display connection handle for '"+ sdcdev +"'");
}

SerdisplibConnection::~SerdisplibConnection()
{}

void SerdisplibConnection::close()
{
    if (m_connection) {
        SDCONN_close(m_connection);
        m_connection = NULL;
    }
}

/* }}} */
/* SerdisplibDisplay {{{ */

SerdisplibDisplay::SerdisplibDisplay(SerdisplibConnection  *connection,
                                     const std::string     &displayname,
                                     const std::string     &optionstring)
    : m_display(NULL)
{
    m_display = serdisp_init(connection->m_connection,
                             displayname.c_str(),
                             optionstring.c_str());
    if (!m_display)
        throw DisplayError("Unable to call serdisp_init()");

    clearBuffer();
}

SerdisplibDisplay::~SerdisplibDisplay()
{
    if (m_display)
        quit();
}

void SerdisplibDisplay::close()
{
    serdisp_close(m_display);
    m_display = NULL;
}

void SerdisplibDisplay::quit()
{
    serdisp_quit(m_display);
    m_display = NULL;
}

void SerdisplibDisplay::clearBuffer()
{
    serdisp_clearbuffer(m_display);
}

void SerdisplibDisplay::clear()
{
    serdisp_clear(m_display);
}

void SerdisplibDisplay::update()
{
    serdisp_update(m_display);
}

void SerdisplibDisplay::rewrite()
{
    serdisp_rewrite(m_display);
}

void SerdisplibDisplay::blink(int what, int cnt, int delta)
{
    serdisp_blink(m_display, what, cnt, delta);
}

int SerdisplibDisplay::getWidth() const
{
    return serdisp_getwidth(m_display);
}

int SerdisplibDisplay::getHeight() const
{
    return serdisp_getheight(m_display);
}

long SerdisplibDisplay::getOption(const std::string &optionName, int &typeSize)
{
    return serdisp_getoption(m_display, optionName.c_str(), &typeSize);
}

void SerdisplibDisplay::setOption(const std::string &optionName, long value)
{
    serdisp_setoption(m_display, optionName.c_str(), value);
}

int SerdisplibDisplay::isOption(const std::string &optionName)
{
    return serdisp_isoption(m_display, optionName.c_str());
}

void SerdisplibDisplay::setPixel(int x, int y, int color)
{
    serdisp_setpixel(m_display, x, y, color);
}

long SerdisplibDisplay::getPixel(int x, int y)
{
    return serdisp_getpixel(m_display, x, y);
}

/* }}} */
/* SerdisplibTextDisplay {{{ */

SerdisplibTextDisplay::SerdisplibTextDisplay(SerdisplibConnection  *connection,
                                             const std::string     &displayname,
                                             const std::string     &optionstring)
    : SerdisplibDisplay(connection, displayname, optionstring)
    , m_locale("C")
{}

int SerdisplibTextDisplay::getRows() const
{
    return getHeight() / (FONT_HEIGHT + GAP_Y);
}

int SerdisplibTextDisplay::getColumns() const
{
    return getWidth() / (FONT_WIDTH + GAP_X);
}

std::string SerdisplibTextDisplay::getCharset() const
{
    return m_charset;
}

void SerdisplibTextDisplay::setCharset(const std::string &charset)
{
    m_charset = charset;
}

void SerdisplibTextDisplay::setLocale(const std::string &locale)
{
    m_locale = locale;
}

void SerdisplibTextDisplay::renderText(int               line,
                                       int               startColumn,
                                       enum DisplayFont  font,
                                       const char        *text, ...)
{
    if (line < 0 || line >= getRows())
        throw std::out_of_range("SerdisplibDisplay::setLineText(): "
                                "The line number has to be between 0 and " + bw::str(getRows()) +
                                "but was " + bw::str(line));

    // format the string we need to render
    std::va_list ap;
    va_start(ap, text);
    std::string formated_text = common::str_vprintf_l(text, m_locale.c_str(), ap);
    va_end(ap);

    std::string convertedString;
    try {
        convertedString = latin12hd44780( toLatin1(formated_text, m_charset) );
    } catch (const CharsetError &err) {
        throw DisplayError("Unable to perform charset conversion: " + std::string(err.what()) );
    }

    const int offset_y = line * (FONT_HEIGHT + GAP_Y);
    const int max_char_index = std::min(convertedString.size(), size_t(getColumns() - startColumn));

    for (int char_index = 0; char_index < max_char_index; ++char_index) {
        const int offset_x = (startColumn + char_index) * (FONT_WIDTH + GAP_X);
        unsigned char c = convertedString[char_index];

        const unsigned char *char_pic;
        if (font == BOLD_FONT)
            char_pic = Font_6x8_bold[c];
        else
            char_pic = Font_6x8[c];

        for (int y = 0; y < FONT_HEIGHT; y++)
            for (int x = 0; x < FONT_WIDTH; x++)
                setPixel(offset_x + x, offset_y + y, bit_is_set(char_pic[y], FONT_WIDTH - 1 - x));
    }
}

void SerdisplibTextDisplay::clearLine(int line)
{
    if (line < 0 || line >= getRows())
        throw std::out_of_range("SerdisplibDisplay::clearLine(): "
                                "The line number has to be between 0 and " + bw::str(getRows()) +
                                "but was " + bw::str(line));

    const int offset_y = line * (FONT_HEIGHT + GAP_Y);
    for (int y = offset_y; y < (offset_y + FONT_HEIGHT + GAP_Y); y++) {
        const int width = getWidth();
        for (int x = 0; x < width; x++)
            setPixel(x, y, 0);
    }
}

/* }}} */

} // end namespace vetero
} // end namespace display
