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
#ifndef VETERO_VETEROD_DATAREADER_H
#define VETERO_VETEROD_DATAREADER_H

#include <libbw/io/serialfile.h>

#include "common/dataset.h"
#include "common/error.h"

namespace vetero {
namespace daemon {

/* DataReader {{{ */

/**
 * \class DataReader
 * \brief Reads and interprets weather data from the serial device
 *
 * This class reads the weather data from the serial device and parses the output. It's used in
 * the main application class to read a data set and put that dataset in the database.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup daemon
 */
class DataReader
{
    public:
        /**
         * \brief Constructor
         *
         * Creates a new DataReader object. Configures the serial device, but doesn't open the port.
         *
         * \param[in] deviceName the name of the serial device
         * \param[in] baud the baud rate
         */
        DataReader(const std::string &deviceName, int baud);

        /**
         * \brief Destructor.
         */
        virtual ~DataReader() {}

    public:
        /**
         * \brief Parses the given configuration files
         *
         * Opens the connection to the serial port.
         *
         * \exception ApplicationError if the port cannot be opened.
         */
        void openConnection()
        throw (common::ApplicationError);

        /**
         * \brief Reads one data set
         *
         * This method blocks until a data set is available (usually every three or four minutes).
         *
         * \return the data set that has been read
         * \exception common::ApplicationError if the data could not be interpreted or if reading from the
         *            serial device failed.
         */
        vetero::common::UsbWde1Dataset read()
        throw (common::ApplicationError);

    protected:
        /**
         * \brief Parses a data line read by DataReader::read()
         *
         * \param[in] line the input line to parse
         * \return the parsed data set
         * \exception common::ApplicationError if the string is not a valid input line
         */
        vetero::common::UsbWde1Dataset parseDataset(const std::string &line) const
        throw (common::ApplicationError);

    private:
        std::string m_serialDeviceName;
        bw::io::SerialFile m_serialDevice;
        int m_baud;
};

/* }}} */

} // end namespace vetero
} // end namespace daemon

#endif // VETERO_VETEROD_DATAREADER_H
