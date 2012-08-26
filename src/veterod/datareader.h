/* {{{
 * (c) 2010-2012, Bernhard Walle <bernhard@bwalle.de>
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
#include "common/configuration.h"

namespace vetero {
namespace daemon {

/* DataReader {{{ */

//
// \class DataReader
// \brief Reads and interprets weather data from the serial device
//
// This class reads the weather data from the serial device and parses the output. It's used in
// the main application class to read a data set and put that dataset in the database.
//
class DataReader
{
    public:
        DataReader(const common::Configuration &configuration);
        virtual ~DataReader() {}

    public:
        void openConnection();
        vetero::common::UsbWde1Dataset read();

    protected:
        vetero::common::UsbWde1Dataset parseDataset(const std::string &line) const;

    private:
        const common::Configuration &m_configuration;
        bw::io::SerialFile m_serialDevice;
};

/* }}} */

} // end namespace vetero
} // end namespace daemon

#endif // VETERO_VETEROD_DATAREADER_H
