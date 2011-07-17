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

#include <libbw/fileutils.h>

#include "nameprovider.h"

namespace vetero {
namespace reportgen {

/* NameProvider {{{ */

// -------------------------------------------------------------------------------------------------
const std::string NameProvider::HTML_EXTENSION(".xhtml");
const std::string NameProvider::SVG_EXTENSION(".svgz");
const std::string NameProvider::INDEX_HTML("index.xhtml");

// -------------------------------------------------------------------------------------------------
NameProvider::NameProvider(const VeteroReportgen &reportgen)
    : m_reportgen(reportgen)
{}

// -------------------------------------------------------------------------------------------------
NameProvider::~NameProvider()
{}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::dailyDir(const bw::Datetime &date) const
{
    return bw::FileUtils::join(
        m_reportgen.configuration().reportDirectory(),
        date.strftime("%Y/%m/%d")
    );
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::dailyDirLink(const bw::Datetime &date) const
{
    return date.strftime("/%Y/%m/%d/");
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::dailyIndex(const bw::Datetime &date) const
{
    return bw::FileUtils::join(dailyDir(date), INDEX_HTML);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::dailyDiagram(const bw::Datetime &date, const std::string &type) const
{
    return bw::FileUtils::join(dailyDir(date), type + SVG_EXTENSION);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::dailyDiagramLink(const bw::Datetime &date, const std::string &type) const
{
    return bw::FileUtils::join(date.strftime("/%Y/%m/%d"), type + SVG_EXTENSION);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::monthlyDir(const bw::Datetime &date) const
{
    return bw::FileUtils::join(
        m_reportgen.configuration().reportDirectory(),
        date.strftime("%Y/%m")
    );
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::monthlyDirLink(const bw::Datetime &date) const
{
    return date.strftime("/%Y/%m/");
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::monthlyIndex(const bw::Datetime &date) const
{
    return bw::FileUtils::join(monthlyDir(date), INDEX_HTML);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::monthlyDiagram(const bw::Datetime &date, const std::string &type) const
{
    return bw::FileUtils::join(monthlyDir(date), type + SVG_EXTENSION);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::monthlyDiagramLink(const bw::Datetime &date, const std::string &type) const
{
    return bw::FileUtils::join(date.strftime("/%Y/%m"), type + SVG_EXTENSION);
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::indexDir() const
{
    return m_reportgen.configuration().reportDirectory();
}

// -------------------------------------------------------------------------------------------------
std::string NameProvider::indexPage() const
{
    return bw::FileUtils::join(indexDir(), INDEX_HTML);
}

/* }}} */

} // end namespace daemon
} // end namespace reportgen
