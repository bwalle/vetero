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

#include <algorithm>

#include "common/utils.h"
#include "validdatacache.h"

namespace vetero {
namespace reportgen {

// -------------------------------------------------------------------------------------------------
ValidDataCache::ValidDataCache(common::DbAccess &dbAccess) throw (common::DatabaseError)
    : m_dbAccess(dbAccess)
{
    m_dataDays = m_dbAccess.dataDays();
    m_dataMonths = m_dbAccess.dataMonths();
}

// -------------------------------------------------------------------------------------------------
ValidDataCache::~ValidDataCache()
{}

// -------------------------------------------------------------------------------------------------
bool ValidDataCache::dataAtDay(int year, int month, int day) const
{
    std::string dayStr = common::str_printf("%04d-%02d-%02d", year, month, day);
    return std::binary_search(m_dataDays.begin(), m_dataDays.end(), dayStr);
}

// -------------------------------------------------------------------------------------------------
bool ValidDataCache::dataInMonth(int year, int month) const
{
    std::string monthStr = common::str_printf("%04d-%02d", year, month);
    return std::binary_search(m_dataMonths.begin(), m_dataMonths.end(), monthStr);
}

} // namespace reportgen
} // namespace vetero
