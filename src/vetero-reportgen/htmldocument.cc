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

#include <fstream>

#include <libbw/stringutil.h>

#include "common/utils.h"
#include "common/translation.h"
#include "htmldocument.h"
#include "config.h"

namespace vetero {
namespace reportgen {

/* HtmlDocument {{{ */

HtmlDocument::HtmlDocument(const VeteroReportgen *reportgen)
    : m_reportgen(reportgen)
    , m_displayTitle(true)
    , m_autoReload(-1)
{}

void HtmlDocument::setTitle(const std::string &title)
{
    m_headerInfo.title = title;
}

void HtmlDocument::setForwardNavigation(const std::string &link, const std::string &linkTitle)
{
    bool active = !link.empty();
    m_headerInfo.forwardLink = generateLink(link, "&#9658;", replaceHtml(linkTitle), active);
    m_headerInfo.haveNavigation = true;
}

void HtmlDocument::setBackwardNavigation(const std::string &link, const std::string &linkTitle)
{
    bool active = !link.empty();
    m_headerInfo.backwardLink = generateLink(link, "&#9668;", replaceHtml(linkTitle), active);
    m_headerInfo.haveNavigation = true;
}

void HtmlDocument::setUpNavigation(const std::string &link, const std::string &linkTitle)
{
    bool active = !link.empty();
    m_headerInfo.upLink = generateLink(link, "&#9650;", replaceHtml(linkTitle), active);
    m_headerInfo.haveNavigation = true;
}

int HtmlDocument::autoReload() const
{
    return m_autoReload;
}

void HtmlDocument::setAutoReload(int reloadTime)
{
    m_autoReload = reloadTime;
}

bool HtmlDocument::displayTitle() const
{
    return m_displayTitle;
}

void HtmlDocument::setDisplayTitle(bool displayTitle)
{
    m_displayTitle = displayTitle;
}

void HtmlDocument::addSection(const std::string &title,
                              const std::string &shortName,
                              const std::string &id)
{
    m_bodyStream << "<h2>" << replaceHtml(title) << "<a name='" << id << "'>&nbsp;</a>" << "</h2>";
    Section section;
    section.id = id;
    section.shortTitle = shortName;
    m_sections.push_back(section);
}

void HtmlDocument::addSectionAsLink(const std::string &title, const std::string &shortName,
                                    const std::string &id, const std::string &targetUrl)
{
    m_bodyStream << "<h2>" << generateLink(targetUrl, replaceHtml(title), "", true)
                 << "<a name='" << id << "'>&nbsp;</a>" << "</h2>";
    Section section;
    section.id = id;
    section.shortTitle = shortName;
    m_sections.push_back(section);
}

void HtmlDocument::startParagraph()
{
    m_bodyStream << "<p>";
}

void HtmlDocument::endParagraph()
{
    m_bodyStream << "</p>";
}

void HtmlDocument::link(const std::string &target, const std::string &name, bool active)
{
    m_bodyStream << generateLink(target, replaceHtml(name), "", active);
}

void HtmlDocument::text(const std::string &text, bool active)
{
    if (active)
        m_bodyStream << replaceHtml(text);
    else
        m_bodyStream << "<span class='inactive'>" << replaceHtml(text) << "</span>";
}

void HtmlDocument::img(const std::string &filename)
{
    m_bodyStream << "<img src=\"" << filename << "\" />";
}

void HtmlDocument::addTopLink()
{
    m_bodyStream << "<a href=\"#top\">^</a>";
}

std::string HtmlDocument::replaceHtml(const std::string &text)
{
    std::string ret(text);

    ret = bw::replace_char(ret, '&', "&amp;");
    ret = bw::replace_char(ret, '<', "&lt;");
    ret = bw::replace_char(ret, '>', "&gt;");

    return ret;
}

bool HtmlDocument::write(const std::string &filename)
{
    std::ofstream htmlFile(filename.c_str());

    if (!htmlFile.is_open())
        return false;

    write(htmlFile);

    return true;
}

void HtmlDocument::write(std::ostream &os)
{
    std::string titleColor1(m_reportgen->configuration().reportTitleColor1());
    std::string titleColor2(m_reportgen->configuration().reportTitleColor2());

    os << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\""
       << " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
       << "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";

    // HTML header
    os << "<head>"
       << "<title>Vetero: " << replaceHtml(m_headerInfo.title) << "</title>";
    writeCss(os);
    os << "<meta http-equiv='content-type' content='text/html; charset=utf-8' />";
    if (m_autoReload > 0)
       os << "<meta http-equiv='refresh' content='" << (m_autoReload*60) << "' />";
    os << "</head>" << std::endl;

    // body start
    os << "<body><a name=\"top\" />" << std::endl;

    // index table
    os << "<table width=\"100%\" bgcolor='" << titleColor1 << "' "
       << "       align=\"center\" cellspacing=\"0\">"
       << "<tr>"
       << "  <td style=\"padding: 10px;"
       << "       border-left-width:1px; border-left-color:#000000; border-left-style:solid;"
       << "       border-right-width:1px; border-right-color:#000000; border-right-style:solid;"
       << "       border-top-width:1px; border-top-color:#000000; border-top-style:solid\">"
       << "    <div align=\"right\" style=\"color: #ffffff; font-size: 30pt; font-weight:bold\">"
       << "       Vetero"
       << "    </div>";

    if (!m_reportgen->configuration().locationString().empty()) {
        os << "    <div align=\"right\" style=\"color: #ffffff; font-size: 13pt; font-style:italic;\">"
           << "       Daten für " << replaceHtml(m_reportgen->configuration().locationString())
           << "    </div>";
    }

    os << "  </td>"
       << "</tr>";

    // index entries
    os << "<tr>"
       << "  <td bgcolor='" << titleColor2 << "'"
       << "      style='border-width:1px; border-color:#000000; border-style:solid; padding: 5px;'>";

    os << " <a href='/'><b>Home</b></a>";
    for (std::vector<Section>::const_iterator it = m_sections.begin(); it != m_sections.end(); ++it) {
        const Section &section = *it;
        os << " | " << "<a href=\"#" << section.id << "\">" << replaceHtml(section.shortTitle) << "</a>";
    }

    if (m_headerInfo.haveNavigation) {
        os << "<span style='position:absolute;right:2em'>"
           << m_headerInfo.backwardLink << "&nbsp;&nbsp;"
           << m_headerInfo.upLink << "&nbsp;&nbsp;"
           << m_headerInfo.forwardLink
           << "</span>\n";
    }

    os << "  </td>"
          "</tr>"
          "</table>";

    if (m_displayTitle)
        os << "<h1>" << replaceHtml(m_headerInfo.title) << "</h1>";

    os << m_bodyStream.str();

    os << "<table width='100%' bgcolor='#217808' align='center' cellspacing='0' style='margin-top: 20px'>\n"
       << "  <tr>\n"
       << "    <td bgcolor='#dddddd' style='padding: 5px;'>\n"
       << common::str_printf( _("Created by %s %s on %s\n"),
                             "<a href='https://bitbucket.org/bwalle/vetero'>Vetero</a> ",
                             GIT_VERSION, bw::Datetime::now().strftime("%Y-%m-%d %H:%M").c_str())
       << "    </td>\n"
       << "  </tr>\n"
       << "</table>\n";
 
    os << "</body></html>";
}

void HtmlDocument::writeCss(std::ostream &os)
{
    os << "<link href='https://fonts.googleapis.com/css?family=Droid+Serif:regular,bold' "
       << "    rel='stylesheet' type='text/css' />\n";

    os << "<style type=\"text/css\">\n";

    os << "body {\n"
       << "    font-family:      Optima, 'Droid Serif', Arial, Helvetica, sans-serif;\n"
       << "    font-size:        10.5pt;\n"
       << "    margin:           15px;\n"
       << "    background-color: #ffffff;\n"
       << "}\n"
       << "span.inactive {\n"
       << "    color:            #888888;\n"
       << "}\n"
       << "h1 {\n"
       << "    margin-top:       30px;\n"
       << "    margin-bottom:    25px;\n"
       << "    border-bottom:    1px solid black;\n"
       << "}\n"
       << "a {\n"
       << "    color:            #000000;\n"
       << "    text-decoration:  none;\n"
       << "}\n"
       << "a:active {\n"
       << "    color:            #000000;\n"
       << "    text-decoration:  underline;\n"
       << "}\n"
       << "a:hover {\n"
       << "    color:            #000000;\n"
       << "    text-decoration:  underline;\n"
       << "}\n";

    os << "</style>" << std::endl;
}

std::string HtmlDocument::generateLink(const std::string &target,
                                       const std::string &name,
                                       const std::string &title,
                                       bool              active)
{
    std::stringstream ss;
    if (active) {
        ss << "<a href='" << target << "' ";
        if (!title.empty())
            ss << "title='" << title << "' ";
        ss << ">" << name << "</a>";
    } else
        ss << "<span class='inactive'>" << name << "</span>";
    return ss.str();
}

/* }}} */

} // end namespace daemon
} // end namespace reportgen
