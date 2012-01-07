/* {{{
 * (c) 2010, Bernhard Walle <bernhard@bwalle.de>
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
#include <cmath>

#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/datetime.h>

#include "common/utils.h"
#include "pressurereader.h"

namespace vetero {
namespace daemon {

/* PressureReader {{{ */

PressureReader::PressureReader(int i2cBus)
    : m_filename( common::str_printf("/sys/bus/i2c/drivers/bmp085/%d-0077/pressure0_input", i2cBus) )
    , m_height(0)
{
    BW_DEBUG_DBG("Pressure sensor device file: '%s'", m_filename.c_str());
}

int PressureReader::height() const
{
    return m_height;
}

void PressureReader::setHeight(int height)
{
    m_height = height;
}

int PressureReader::readPressure()
{
    int pressure;
    std::ifstream fin(m_filename.c_str());
    fin >> pressure;

    if (!fin.good())
        throw common::ApplicationError("Unable to read pressure from '" + m_filename + "'");

    return calculateSeaLevelPressure(pressure);
}

int PressureReader::calculateSeaLevelPressure(int pressure) const
{
    if (m_height == 0)
        return pressure;

    double floatPressure(pressure);
    floatPressure /= 100.0;

    double correctedPressure = floatPressure / pow(1.0 - m_height/44330.0, 5.255);
    BW_DEBUG_DBG("calculateSeaLevelPressure(%lf) = %lf", floatPressure, correctedPressure);

    return round(correctedPressure*100);
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
