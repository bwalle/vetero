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
#include <unistd.h>

#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>
#include <libbw/datetime.h>

#include "datareader.h"

namespace vetero {
namespace daemon {

/* DataReader {{{ */

DataReader *DataReader::create(const common::Configuration &configuration)
{
    BW_DEBUG_STREAM_DBG("Sensor type " << configuration.sensorType());
    if (configuration.sensorType() == common::SensorType::FreeTec)
        return new FreeTecDataReader(configuration);
    else
        return new UsbWde1DataReader(configuration);
}

DataReader::DataReader(const common::Configuration &configuration)
    : m_configuration(configuration)
{}

/* }}} */
/* UsbWde1DataReader {{{ */

UsbWde1DataReader::UsbWde1DataReader(const common::Configuration &configuration)
    : DataReader(configuration),
      m_serialDevice(configuration.serialDevice())
{}

void UsbWde1DataReader::openConnection()
{
    if (!m_serialDevice.openPort())
        throw common::ApplicationError("Unable to open port '"+ m_configuration.serialDevice() + "': " +
                               m_serialDevice.getLastError());

    if (!m_serialDevice.reconfigure(m_configuration.serialBaud(), bw::io::SerialFile::FC_NONE))
        throw common::ApplicationError("Unable to configure serial port: " + m_serialDevice.getLastError());

    BW_DEBUG_INFO("Connection to serial port etablished.");
}

vetero::common::Dataset UsbWde1DataReader::read()
{
    std::string line;

    // don't treat empty lines as an error
    do {
        try {
            BW_DEBUG_DBG("Waiting for input on the serial line");
            line = m_serialDevice.readLine();
        } catch (const bw::IOError &ioe) {
            throw common::ApplicationError("Unable to read from serial device: " +
                                   std::string(ioe.what()) );
        }
        line = bw::strip(line);
    } while (line.empty());

    BW_DEBUG_DBG("Read line '%s'", line.c_str());

    if (!bw::startsWith(line, "$1"))
        throw common::ApplicationError("Received data doesn't start with '$1', maybe not in LogView "
                               "mode? (" + line + ")");

    vetero::common::Dataset data = parseDataset(line);
    data.setTimestamp(bw::Datetime::now());
    data.setSensorType(m_configuration.sensorType());

    BW_DEBUG_DBG("Read dataset: %s", data.str().c_str());

    return data;
}

vetero::common::Dataset UsbWde1DataReader::parseDataset(const std::string &line) const
{
    std::vector<std::string> parts = bw::stringsplit(line, ";");
    if (parts.size() != 25)
        throw common::ApplicationError("Invalid data set received: " + bw::str(parts.size()) +
                               " instead of 25 parts.");

    vetero::common::Dataset data;

    if (m_configuration.sensorType() == common::SensorType::Kombi) {

        const int TEMPERATURE_INDEX = 19;
        const int HUMIDITY_INDEX    = 20;
        const int WIND_INDEX        = 21;
        const int RAIN_INDEX        = 22;
        const int IS_RAIN_INDEX     = 23;


        // temperature
        std::string temperature = parts[TEMPERATURE_INDEX];
        temperature = bw::replace_char(temperature, ',', "");
        data.setTemperature( bw::from_str<int>(temperature) * 10 );

        // humidity
        std::string humidity = parts[HUMIDITY_INDEX];
        data.setHumidity( bw::from_str<int>(humidity) * 100 );

        // wind
        std::string wind = parts[WIND_INDEX];
        wind = bw::replace_char(wind, ',', "");
        data.setWindSpeed( bw::from_str<int>(wind) * 10 );

        // rain
        std::string rain = parts[RAIN_INDEX];
        data.setRainGauge( bw::from_str<int>(rain) );

        // is raining
        std::string isRain = parts[IS_RAIN_INDEX];
        data.setIsRain( bw::from_str<int>(isRain) );

    } else if (m_configuration.sensorType() == common::SensorType::Normal) {

        const int TEMPERATURE_INDEX_1 = 3;
        const int HUMIDITY_INDEX_1 = 11;

        // computer scientists start counting from 0, not from 1 :-)
        int number = m_configuration.sensorNumber() - 1;

        // temperature
        std::string temperature = parts[TEMPERATURE_INDEX_1 + number];
        temperature = bw::replace_char(temperature, ',', "");
        data.setTemperature( bw::from_str<int>(temperature) * 10 );

        // humidity
        std::string humidity = parts[HUMIDITY_INDEX_1 + number];
        data.setHumidity( bw::from_str<int>(humidity) * 100 );
    }

    return data;
}

/* }}} */
/* FreeTecDataReader {{{ */

FreeTecDataReader::FreeTecDataReader(const common::Configuration &configuration)
    : DataReader(configuration)
{}

void FreeTecDataReader::openConnection()
{
    static constexpr unsigned int VENDOR_ID = 0x1941;
    static constexpr unsigned int PRODUCT_ID = 0x8021;

    usb::UsbManager &manager = usb::UsbManager::instance();
    manager.detectDevices();

    usb::Device *weatherStation = nullptr;

    for (size_t i = 0; i < manager.getNumberOfDevices() && weatherStation == nullptr; i++) {
        usb::Device *device = manager.getDevice(i);
        usb::DeviceDescriptor descriptor = device->getDescriptor();

        int vendorId = descriptor.getVendorId();
        int productId = descriptor.getProductId();

        BW_DEBUG_DBG("Checking USB device %04X:%04X", vendorId, productId);

        if ((vendorId == VENDOR_ID) && (productId == PRODUCT_ID))
            weatherStation = device;
    }

    if (!weatherStation)
        throw common::ApplicationError("Weather station USB device not found");

    usb::DeviceHandle *usb_handle = weatherStation->open();

    try {
        std::unique_ptr<usb::ConfigDescriptor> configDescriptor(weatherStation->getConfigDescriptor(0));
        BW_DEBUG_TRACE("usb::DeviceHandle::setConfiguration(%d)", configDescriptor->getConfigurationValue());
        usb_handle->setConfiguration(configDescriptor->getConfigurationValue());
    } catch (const usb::Error &err) {
        if (err.code() != 6)
            BW_ERROR_WARNING("Unable to set configuration: %s", err.what());
        else
            throw common::ApplicationError("Unable to set configuration: " + std::string(err.what()));
    }

    unsigned int interfaceNumber;
    try {
        std::unique_ptr<usb::ConfigDescriptor> configDescriptor(weatherStation->getConfigDescriptor(0));
        std::unique_ptr<usb::InterfaceDescriptor> interfaceDescriptor(configDescriptor->getInterfaceDescriptor(0, 0));
        interfaceNumber = interfaceDescriptor->getInterfaceNumber();

        try {
            BW_DEBUG_TRACE("usb::DeviceHandle::detachKernelDriver(%d)", interfaceNumber);
            usb_handle->detachKernelDriver(interfaceNumber);
        } catch (const usb::Error &err) {
            BW_DEBUG_DBG( "Unable to claim interface: %s", err.what());
        }

        BW_DEBUG_TRACE("usb::DeviceHandle::claimInterface(%d)", interfaceNumber);
        usb_handle->claimInterface(interfaceNumber);
    } catch (const usb::Error &err) {
        throw common::ApplicationError("Unable to claim interface: " + std::string(err.what()));
    }
  

    m_handle.reset(usb_handle);

    m_nextRead = time(nullptr);
}


vetero::common::Dataset FreeTecDataReader::read()
{
    vetero::common::Dataset data;
    data.setTimestamp(bw::Datetime::now());
    data.setSensorType(m_configuration.sensorType());

    time_t now = time(nullptr);
    BW_DEBUG_TRACE("now = %lld, next_read=%lld", (unsigned long long)now, (unsigned long long)m_nextRead);
    while (now < m_nextRead) {
	    sleep(m_nextRead - now);
	    now = time(nullptr);
    }

    unsigned char fixed_block[BLOCK_SIZE];
    unsigned char current_block[BLOCK_SIZE];

    readBlock(0, fixed_block);
    if (fixed_block[0] != 0x55)
        throw common::ApplicationError("Bad data returned");

    // Bytes 31 and 32 when combined create an unsigned short int
    // that tells us where to find the weather data we want
    size_t curpos = fixed_block[31] << 8 | fixed_block[30];
    BW_DEBUG_TRACE("Current block offset %x", curpos);

    readBlock(curpos, current_block);

    data.setHumidity(current_block[4] * 100);

    unsigned char tlsb = current_block[5];
    unsigned char tmsb = current_block[6] & 0x7f;
    unsigned char tsign = current_block[6] >> 7;
    double outdoor_temperature = (tmsb * 256 + tlsb) * 0.1;
    if (tsign)
        outdoor_temperature *= -1;

    data.setTemperature(outdoor_temperature*100);

    unsigned char wind = current_block[9];
    unsigned char wind_extra = current_block[11];
    unsigned char wind_dir = current_block[12];

    data.setWindSpeed( (wind + ((wind_extra & 0x0F) << 8)) * 0.36 * 100 );
    data.setWindDirection( wind_dir * 22.5 );
    data.setRainGauge(current_block[14] << 8 | current_block[13]);

    m_nextRead = now + 5*60;

    BW_DEBUG_STREAM_DBG("Read data" << data);
    return data;
}

void FreeTecDataReader::readBlock(size_t offset, unsigned char block[BLOCK_SIZE])
{
    unsigned char least_significant_bit = offset & 0xFF;
    unsigned char most_significant_bit = (offset >> 8) & 0xFF;

    unsigned char msg[] = {
        0xA1,
        most_significant_bit,
        least_significant_bit,
        32,
        0xA1,
        most_significant_bit,
        least_significant_bit,
        32
    };

    static constexpr int timeout = 1000; // ms

    BW_DEBUG_DBG("Setting up reading block offset %d", offset);

    m_handle->controlTransfer(0x21,           // bmRequestType,
                              0x09,           // bRequest,
                              0x200,          // wValue,
                              0,              // wIndex,
                              msg,            // data
                              sizeof(msg),    // size
                              timeout);       // timeout

    BW_DEBUG_DBG("Reading block offset %d", offset);

    int transferred = 0;
    m_handle->bulkTransfer(0x81,              // endpoint
                           block,             // data
                           BLOCK_SIZE,        // length
                           &transferred,      // transferred data
                           timeout);          // timeout

    if (transferred != BLOCK_SIZE)
        throw common::ApplicationError("Unable to read " + std::to_string(BLOCK_SIZE) + " bytes of data. Only " +
                                       std::to_string(transferred) + " bytes read.");
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
