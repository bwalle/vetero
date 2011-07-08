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
#ifndef VETERO_DISPLAYD_SERDISPLIBDISPLAY_H_
#define VETERO_DISPLAYD_SERDISPLIBDISPLAY_H_

#include <stdexcept>
#include <string>

#include <libbw/noncopyable.h>
#include <libbw/compiler.h>

#include <serdisplib/serdisp.h>

namespace vetero {
namespace display {

/* DisplayError {{{ */

/**
 * @class DisplayError
 * @brief Exception class used for errors when accessing the display
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class DisplayError : public std::runtime_error {

    public:
        /**
         * @brief Constructor
         *
         * Creates a new DisplayError.
         *
         * @param[in] string the error string
         */
        DisplayError(const std::string &string)
            : std::runtime_error(string) {}

        /**
         * @brief Destructor
         */
        virtual ~DisplayError()
        throw () {}
};

/* }}} */
/* SerdisplibConnection {{{ */

class SerdisplibDisplay;

/**
 * @class SerdisplibConnection
 * @brief Wrapper for serdisp_CONN_t used in SerdisplibDisplay
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class SerdisplibConnection : private bw::Noncopyable {

    friend class SerdisplibDisplay;

    public:
        /**
         * @brief Constructor
         *
         * Creates a new display connection. See
         * http://serdisplib.sourceforge.net/docs/index.html#serdisp_connect__SDCONN_open
         * for a list of supported connections.
         *
         * @param[in] sdcdev the connection string
         * @exception DisplayError if the connection cannot be created
         */
        SerdisplibConnection(const std::string &sdcdev)
        throw (DisplayError);

        /**
         * @brief Destructor
         *
         * Destroys the connection.
         */
        virtual ~SerdisplibConnection();

    public:
        /**
         * @brief Closes the device occupied by serdisp
         */
        void close();

    private:
        serdisp_CONN_t *m_connection;
};

/* }}} */
/* SerdisplibDisplay {{{ */

/**
 * @class SerdisplibDisplay
 * @brief Wrapper for serdisplib plus simple font rendering
 *
 * This class wraps C serdisplib code and implements simple font rendering using a fixed font from
 * lcd4linux.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class SerdisplibDisplay : private bw::Noncopyable {

    public:
        /**
         * @brief Enumeration value for setOption()
         */
        enum OptionValue {
            OPTION_NO       = SD_OPTION_NO,       /**< switch off an option */
            OPTION_YES      = SD_OPTION_YES,      /**< switch on an option */
            OPTION_TOGGLE   = SD_OPTION_TOGGLE    /**< toggle an option */
        };

    public:
        /**
         * @brief Constructor
         *
         * See also
         * http://serdisplib.sourceforge.net/docs/index.html#serdisp_control__serdisp_init
         * for a description of the parameters.
         *
         * @param[in] connection the connection to be used (must be valid during the whole lifetime
         *            of SerdisplibDisplay)
         * @param[in] displayname the name
         * @param[in] optionstring the options
         * @exception DisplayError if the display handle cannot be created
         */
        SerdisplibDisplay(SerdisplibConnection  *connection,
                          const std::string     &displayname,
                          const std::string     &optionstring)
        throw (DisplayError);

        /**
         * @brief Destructor.
         *
         * Calls quit() if close() hasn't been called earlier.
         */
        virtual ~SerdisplibDisplay();

    public:

        /**
         * @brief Closes the display without clearing / switching it off. output device remains opened
         *
         * this function may for example be used for programs that want to output something and than
         * exit, but without clearing the display (for this, SDCONN_close() shouldn't either be
         * called)
         *
         * @warning Read
         * http://serdisplib.sourceforge.net/docs/index.html#serdisp_control__serdisp_close.
         */
        void close();

        /**
         * @brief clears and switches off the display and releases the output device
         */
        void quit();

        /**
         * @brief resets the internal display-buffer that is used by serdisplib
         *
         * Display will NOT be redrawn!
         */
        void clearBuffer();

        /**
         * @brief clears the whole display
         */
        void clear();

        /**
         * @brief updates the whole display
         */
        void update();

        /**
         * @brief rewrites the whole display
         */
        void rewrite();

        /**
         * @brief blinks the the display
         *
         * @param[in] what 0: blinking using backlight, 1: blinking using display reversing
         * @param[in] cnt often should there be blinking
         * @param[in] delta delay between two blinking intervals
         */
        void blink(int what, int cnt, int delta);

        /**
         * @brief gets width of display
         *
         * @return the width in pixels
         */
        int getWidth() const;

        /**
         * @brief gets height of display
         *
         * @return the height in pixels
         */
        int getHeight() const;

        /**
         * @brief returns the value of the option
         *
         * @param[in] optionName name of option to get
         * @param[out] typeSize the type size of the option to get
         * @return the option value
         */
        long getOption(const std::string &optionName, int &typeSize);

        /**
         * @brief sets the value of a display option
         *
         * @param[in] optionName name of option to set
         * @param[in] value value of option to set, see enum OptionValue
         */
        void setOption(const std::string &optionName, long value);

        /**
         * @brief tests if option is supported
         *
         * @param[in] optionName the name of the option
         * @return 1 option is supported and read/writeable, -1 option is supported but read-only,
         *         0 ption is not supported
         */
        int isOption(const std::string &optionName);

        /**
         * @brief changes a pixel in the display buffer
         *
         * @param[in] x x-position
         * @param[in] y y-position
         * @param[in] color 0: clear (white), <>0: set (black); else: up to 16m colours
         *            (hardware dependend)
         */
        void setPixel(int x, int y, int color);

        /**
         * @brief gets the colour value at position (x/y)
         *
         * @param[in] x x-position
         * @param[in] y y-position
         * @return returns the hardware dependend colour information at (x/y)
         */
        long getPixel(int x, int y);

    private:
        serdisp_t *m_display;
};

/* }}} */
/* DisplayFont {{{ */

/**
 * @brief Font to be used for SerdisplibTextDisplay::renderText().
 *
 * @ingroup display
 */
enum DisplayFont {
    NORMAL_FONT = 0,                /**< use the default 6x8 font */
    BOLD_FONT   = (1<<0)            /**< use the bold 6x8 font */
};

/* }}} */
/* SerdisplibTextDisplay {{{ */

/**
 * @class SerdisplibTextDisplay
 * @brief Display that renders text
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup display
 */
class SerdisplibTextDisplay : public SerdisplibDisplay {

    public:
        /// Number of horizontal pixels of the builtin font.
        const static int FONT_WIDTH = 6;

        /// Number of the vertical pixels of the builtin font.
        const static int FONT_HEIGHT = 8;

        /// Number of pixels between characters in horizontal direction.
        const static int GAP_X = 0;

        /// Number of pixels between characters in vertical direction.
        const static int GAP_Y = 1;

    public:
        /**
         * @copydoc SerdisplibDisplay
         */
        SerdisplibTextDisplay(SerdisplibConnection  *connection,
                              const std::string     &displayname,
                              const std::string     &optionstring)
        throw (DisplayError);

    public:
        /**
         * @brief Returns the number of rows
         *
         * @return the number of rows
         * @sa getHeight()
         */
        int getRows() const;

        /**
         * @brief Returns the number of columns
         *
         * @return the number of columns
         * @sa getWidth()
         */
        int getColumns() const;

        /**
         * @brief Returns the current charset used for text sendering
         *
         * @return the current charset. An empty string means to use the native charset of the
         *         application.
         * @sa setCharset()
         */
        std::string getCharset() const;

        /**
         * @brief Sets the current charset used for text rendering
         *
         * @param[in] charset the character set in which @p text in renderText() is encoded. If
         *            <tt>""</tt> is given (the default), then the native character set as returned
         *            by queryNativeCharset() is used. Be aware that <tt>setlocale(LC_CTYPE,
         *            "")</tt> has to be invoked by the application before calling setLineText(),
         *            otherwise the "native" character set is @b always us-ascii.
         */
        void setCharset(const std::string &charset);

        /**
         * @brief Sets the locale for the text rendering
         *
         * This influences the decimal separator used for printf(). See
         * vetero::common::str_printf_l() for details.
         *
         * @param[in] locale the locale string
         */
        void setLocale(const std::string &locale);

        /**
         * @brief Renders text in the specified line
         *
         * @note The builtin font can render only a subset of latin1, mostly ascii and German
         *       umlauts.
         *
         * @param[in] line the line number, starting with 0
         * @param[in] startColumn the column to start with rendering
         * @param[in] font the font to use, either NORMAL_FONT or BOLD_FONT.
         * @param[in] text the format string for text to render as understood by printf(3)
         * @exception std::out_of_range if @p line is out of range
         * @exception DisplayError if it was not possible to convert @p text to the display charset
         * @sa clearLine()
         * @sa setCharset()
         */
        void renderText(int              line,
                        int              startColumn,
                        enum DisplayFont font,
                        const char       *text, ...)
        throw (std::out_of_range, std::bad_alloc, DisplayError)
        BW_COMPILER_PRINTF_FORMAT(5, 6);

        /**
         * @brief Deletes the contents of a line.
         *
         * @param[in] line the line number, starting with 0.
         * @exception std::out_of_range if @p line is out of range
         */
        void clearLine(int line)
        throw (std::out_of_range);

    private:
        std::string m_charset;
        std::string m_locale;
};

/* }}} */
/* Helper function bit_is_set {{{ */

/**
 * @brief Checks if the bit @p bit is set in @p value
 *
 * @param[in] value the numeric value to check for
 * @param[in] bit the number of the bit
 * @return @c true if the bit is set, @c false otherwise.
 * @ingroup display
 */
template <typename T>
static bool bit_is_set(const T &value, size_t bit)
{
    return value & (1 << bit);
}

/* }}} */

} // end namespace vetero
} // end namespace display

#endif // VETERO_DISPLAYD_SERDISPLIBDISPLAY_H_
