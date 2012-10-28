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
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

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
    // For Linux sysfs files it's better to read them direclty using POSIX syscalls
    // That way we have more control over error handling.

    int fd = open(m_filename.c_str(), O_RDONLY);
    if (fd < 0)
        throw common::SystemError("Unable to open '" + m_filename + "'", errno);

    char buffer[1024];
    ssize_t readChars = -1;

    for (int retryCount = 10; retryCount > 0; --retryCount) {
        readChars = read(fd, buffer, sizeof(buffer) - 1);
        if (readChars > 0)
            break;
        else if (errno != EIO && errno != EBUSY)
            break;

        BW_DEBUG_DBG("Unable to read pressure (%s), retrying", strerror(errno));
        lseek(fd, 0, SEEK_SET); // rewind
        sleep(1); // wait before retrying
    }

    close(fd);
    if (readChars < 0)
        throw common::SystemError("Unable to read from '" + m_filename + "'", errno);

    buffer[readChars] = '\0';

    int pressure = bw::from_str<int>(bw::stripr(buffer));
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
