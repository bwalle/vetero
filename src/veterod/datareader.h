/* {{{
 * (c) 2010-2018, Bernhard Walle <bernhard@bwalle.de>
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

#include <memory>
#include <ctime>
#include <libbw/io/serialfile.h>
#include <usbpp/usbpp.h>

#include "common/dataset.h"
#include "common/error.h"
#include "common/configuration.h"
#include "common/datasocket.h"

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
        static DataReader *create(const common::Configuration &configuration);

    public:
        DataReader(const common::Configuration &configuration);
        virtual ~DataReader() {}

    public:
        virtual void openConnection() = 0;
        virtual vetero::common::Dataset read() = 0;

    protected:
        const common::Configuration &m_configuration;
};

/* }}} */
/* UsbWde1DataReader {{{ */

class UsbWde1DataReader : public DataReader
{
    public:
        UsbWde1DataReader(const common::Configuration &configuration);

    public:
        void openConnection() override;
        vetero::common::Dataset read() override;

    protected:
        vetero::common::Dataset parseDataset(const std::string &line) const;

    private:
        bw::io::SerialFile m_serialDevice;
};

/* }}} */
/* FreeTecDataReader */

class FreeTecDataReader : public DataReader
{
    public:
        FreeTecDataReader(const common::Configuration &configuration);

    public:
        void openConnection() override;
        vetero::common::Dataset read() override;

    protected:
	static constexpr size_t BLOCK_SIZE = 32;
	void readBlock(size_t offset, unsigned char block[BLOCK_SIZE]);

    private:
	std::unique_ptr<usb::DeviceHandle> m_handle;
	time_t m_nextRead;
};

/* }}} */

/* Ws980DataReader */

class Ws980DataReader : public DataReader
{
    public:
        Ws980DataReader(const common::Configuration &configuration);

    public:
        void openConnection() override;
        vetero::common::Dataset read() override;

    private:
        vetero::common::DataSocket connect();

    private:
        static const int WS980_PORT = 45000;
        time_t m_nextRead;
};

/* }}} */


} // end namespace vetero
} // end namespace daemon

#endif // VETERO_VETEROD_DATAREADER_H
