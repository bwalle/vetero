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
#ifndef HTMLDOCUMENT_H_
#define HTMLDOCUMENT_H_

#include <string>
#include <sstream>
#include <iostream>

#include "vetero_reportgen.h"

namespace vetero {
namespace reportgen {

/* HtmlDocument {{{ */

/**
 * @brief Simplify creation of simple HTML documents in C++
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup report
 */
class HtmlDocument
{
    public:
        /**
         * @brief Creates a new HtmlDocument
         *
         * @param[in] reportgen the reportgen object
         */
        HtmlDocument(const VeteroReportgen *reportgen);

    public:
        /**
         * @brief Returns the title of the HTML document
         *
         * @return the title string set with setTitle()
         */
        std::string title() const;

        /**
         * @brief Sets the title for the HTML document
         *
         * @param[in] title the new title for the document
         */
        void setTitle(const std::string &title);

        /**
         * @brief Returns the auto-reload time
         *
         * @return -1 if auto-reload is disabled, else the auto-reload time in minutes.
         */
        int autoReload() const;

        /**
         * @brief Sets the auto-reload time
         *
         * Setting the auto-reload time generates a http-equiv tag which instructs the browser
         * to automatically reload the given site.
         *
         * @param[in] reloadTime the auto-reload time in minutes or -1 to disable auto-reloading
         *            (which is also the default setting)
         */
        void setAutoReload(int reloadTime);

        /**
         * @brief Checks if the title is displayed
         *
         * If the property is set, then the title is not only set as HTML document title
         * but rendered as title in the document. The default is true.
         *
         * @return @c true if the property is set, @c false otherwise.
         */
        bool displayTitle() const;

        /**
         * @brief Sets the property if the title should be rendered
         *
         * @param[in] displayTitle @c true if the property is set, @c false otherwise
         * @see displayTitle()
         */
        void setDisplayTitle(bool displayTitle);

        /**
         * @brief Adds a new section for the document
         *
         * This inserts a title in the HTML document part and also adds an anchor and an entry
         * in the index bar on the top.
         *
         * @param[in] title the title string that is displayed in the document
         * @param[in] shortName the short name that is displayed in the index
         * @param[in] id a string suitable for the HTML anchor
         */
        void addSection(const std::string &title, const std::string &shortName, const std::string &id);

        /**
         * @brief Inserts the paragraph start tag
         *
         * Inserts <tt>&lt;p&gt;</tt>.
         */
        void startParagraph();

        /**
         * @brief Inserts the paragraph end tag
         *
         * Inserts <tt>&lt;p&gt;</tt>.
         */
        void endParagraph();

        /**
         * @brief Inserts a link
         *
         * Inserts <tt>&lt;a href="target"&gt;name&lt;/a&gt;</tt>.
         *
         * @param[in] target the link target
         * @param[in] name the human-readable link name, replaceHtml() is called.
         * @param[in] active if @c false, then the link is not generated as link but as grey HTML
         *            to show the difference between a link and no link
         * @return a self reference
         */
        void link(const std::string &target, const std::string &name, bool active=true);

        /**
         * @brief Makes it easy to print inactive text
         *
         * @param[in] text the text to print
         * @param[in] active if @c false, then the text is gray
         */
        void text(const std::string &text, bool active=true);

        /**
         * @brief Inserts a image tage
         *
         * Inserts <tt>&lt;img src="filename" /&gt;</tt>.
         *
         * @param[in] filename the name of the image file
         */
        void img(const std::string &filename);

        /**
         * @brief Adds a link to the top location
         */
        void addTopLink();

        /**
         * @brief Output operator
         *
         * Used to print simple text, integers, etc. Example:
         *
         * @code
         * HtmlDocument bla;
         *
         * bla << "test=" << fasel << std::endl;
         * @endcode
         *
         * @param[in] t the element that should be printed. Since the underlying stream is
         *            std::ostream, all types that have a valid operator<< for std::ostream can be
         *            printed here. HTML escaping is NOT done (see replaceHtml()).
         * @return a reference to self for concatenation
         * @see addHtml()
         */
        template<typename T>
        HtmlDocument &operator<<(const T &t);

        /**
         * @brief Replaces special characters for HTML
         *
         * Replaces @c < with <tt>&amp;lt;</tt>, > with <tt>&amp;gt;</tt> and <tt>&</tt> with
         * <tt>&amp;amp;</tt>.
         *
         * @param[in] text the string that should be escaped
         * @return the resulting HTML
         */
        std::string replaceHtml(const std::string &text) const;

        /**
         * @brief Writes the final result to the specified file
         *
         * @param[in] filename the name of the file
         * @return @c true on success, @c false on failure.
         */
        bool write(const std::string &filename);

    protected:
        /**
         * @brief Writes the final result to the specified stream
         *
         * @param[in] os the output stream
         */
        void write(std::ostream &os);

        /**
         * @brief Writes the CSS definitions to the given stream
         *
         * @param[in] os the output stream
         */
        void writeCss(std::ostream &os);

    private:
        const VeteroReportgen *m_reportgen;
        struct Section {
            std::string id;
            std::string shortTitle;
        };
        std::string m_title;
        std::stringstream m_bodyStream;
        std::vector<Section> m_sections;
        bool m_displayTitle;
        int m_autoReload;
};

/* }}} */
/* Template implementation {{{ */

// -------------------------------------------------------------------------------------------------
template<typename T>
HtmlDocument &HtmlDocument::operator<<(const T &t)
{
    m_bodyStream << t;
    return *this;
}

/* }}} */

} // end namespace daemon
} // end namespace reportgen

#endif /* HTMLDOCUMENT_H_ */
