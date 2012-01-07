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

#include "reportgenerator.h"

namespace vetero {
namespace reportgen {

ReportGenerator::ReportGenerator(VeteroReportgen *reportgen)
    : m_reportgen(reportgen)
    , m_lockfile(reportgen->configuration().reportDirectory())
    , m_nameProvider(*reportgen)
{
    if (!m_lockfile.lockShared())
        throw common::ApplicationError("Unable to obtain lock: " + m_lockfile.error());
}

ReportGenerator::~ReportGenerator()
{}

VeteroReportgen *ReportGenerator::reportgen() const
{
    return m_reportgen;
}

const NameProvider &ReportGenerator::nameProvider() const
{
    return m_nameProvider;
}

} // end namespace reportgen
} // end namespace vetero
