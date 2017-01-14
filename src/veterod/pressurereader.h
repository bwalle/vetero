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
#ifndef VETERO_VETEROD_PRESSUREREADER_H
#define VETERO_VETEROD_PRESSUREREADER_H

#include <libbw/io/serialfile.h>

#include "common/dataset.h"
#include "common/error.h"

namespace vetero {
namespace daemon {

/* PressureReader {{{ */

/**
 * \class PressureReader
 * \brief Reads pressure from a bmp085 pressure sensor (Linux sysfs)
 *
 * The pressure data must be made available by the bmp085 driver of the Linux kernel.
 * Since the bmp085 device always has the I2C address 0x77, the only configuration
 * option is the I2C bus number.
 *
 * So if the bus number is %d, then the file that will be accessed by the PressureReader
 * is <tt>"/sys/bus/i2c/drivers/bmp085/%d-0077/pressure0_input"</tt> (printf-style format
 * string).
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup daemon
 */
class PressureReader
{
    public:
        /**
         * \brief Constructor
         *
         * Creates a new PressureReader object
         *
         * \param[in] i2cBus the I2C bus number
         */
        PressureReader(int i2cBus);

    public:
        /**
         * \brief Returns the height that is used for pressure correction.
         *
         * The default height is zero.
         *
         * \return the height in m
         */
        int height() const;

        /**
         * \brief Sets the height that is used for pressure correction
         *
         * The pressure correction algorithm from the data sheet of the bmp085 is used
         * to calculate the corrected pressure.
         *
         * \param[in] height the new height in m
         */
        void setHeight(int height);

        /**
         * \brief Reads one pressure
         *
         * This method may block, but only a few seconds.
         *
         * \return the data set that has been read
         * \exception common::ApplicationError if the data could not be read.
         */
        int readPressure();

    protected:
        /**
         * \brief Does the pressure correction to get the pressure at sea level
         *
         * Takes the height set with setHeight() and the given pressure and calculates the pressure
         * at sea level. The formula from the BMP085 data sheet is used:
         *
         * <tt>p0 = p / ((1 - alt/44330) ^ 5.255)</tt>
         *
         * \param[in] pressure the pressure in 1/100 hPa
         * \return the corrected pressure in 1/100 hPa
         */
        int calculateSeaLevelPressure(double pressure) const;

    private:
        std::string m_filename;
        int m_height;
};

/* }}} */

} // end namespace vetero
} // end namespace daemon

#endif // VETERO_VETEROD_PRESSUREREADER_H
