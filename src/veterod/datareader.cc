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

#include <cmath>

#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>
#include <libbw/datetime.h>

#include <common/weather.h>

#include "datareader.h"

namespace vetero {
namespace daemon {

/* DataReader {{{ */

DataReader *DataReader::create(const common::Configuration &configuration)
{
    BW_DEBUG_STREAM_DBG("Sensor type " << configuration.sensorType());
    if (configuration.sensorType() == common::SensorType::FreeTec)
        return new FreeTecDataReader(configuration);
    else if (configuration.sensorType() == common::SensorType::Ws980)
        return new Ws980DataReader(configuration);
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
    time_t now = time(nullptr);
    BW_DEBUG_TRACE("now = %lld, next_read=%lld", (unsigned long long)now, (unsigned long long)m_nextRead);
    while (now < m_nextRead) {
	    sleep(m_nextRead - now);
	    now = time(nullptr);
    }

    vetero::common::Dataset data;
    data.setTimestamp(bw::Datetime::now());
    data.setSensorType(m_configuration.sensorType());

    unsigned char fixed_block[BLOCK_SIZE];
    unsigned char current_block[BLOCK_SIZE];

    readBlock(0, fixed_block);
    if (fixed_block[0] != 0x55)
        throw common::ApplicationError("Bad data returned");

    // Bytes 31 and 32 when combined create an unsigned short int
    // that tells us where to find the weather data we want
    size_t curpos = fixed_block[31] << 8 | fixed_block[30];
    BW_DEBUG_TRACE("Current block offset %zx", curpos);

    readBlock(curpos, current_block);

    data.setHumidity(current_block[4] * 100);

    unsigned char tlsb = current_block[5];
    unsigned char tmsb = current_block[6] & 0x7f;
    unsigned char tsign = current_block[6] >> 7;
    double outdoor_temperature = (tmsb * 256 + tlsb) * 0.1;
    if (tsign)
        outdoor_temperature *= -1;

    data.setTemperature(outdoor_temperature*100);

    uint16_t pressure = current_block[7] | (current_block[8] << 8);
    using common::weather::calculateSeaLevelPressure;
    int seaLevelPressure = round(calculateSeaLevelPressure(m_configuration.pressureHeight(), pressure/10.0) * 100.0);

    data.setPressure(seaLevelPressure);

    unsigned char wind = current_block[9];
    unsigned char gust = current_block[10];
    unsigned char wind_extra = current_block[11];
    unsigned char wind_dir = current_block[12];

    data.setWindSpeed( (wind + ((wind_extra & 0x0F) << 8)) * 0.38 * 100 );
    data.setWindGust(  (gust + ((wind_extra & 0xF0) << 4)) * 0.38 * 100 );
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

    BW_DEBUG_DBG("Setting up reading block offset %zd", offset);

    m_handle->controlTransfer(0x21,           // bmRequestType,
                              0x09,           // bRequest,
                              0x200,          // wValue,
                              0,              // wIndex,
                              msg,            // data
                              sizeof(msg),    // size
                              timeout);       // timeout

    BW_DEBUG_DBG("Reading block offset %zd", offset);

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
/* Ws980DataReader {{{ */

Ws980DataReader::Ws980DataReader(const common::Configuration &configuration)
    : DataReader(configuration)
{}

void Ws980DataReader::openConnection()
{
    static const unsigned char get_version[] = { 0xff, 0xff, 0x50, 0x03, 0x53 };

    // just get the version number to test the connection
    auto socket = connect();
    socket.write(get_version, sizeof(get_version));

    char version_response[256];
    memset(&version_response, 0, sizeof (version_response));
    ssize_t version_len = socket.read(version_response, sizeof(version_response) - 1);

    std::string version = std::string(version_response + 5);
    BW_DEBUG_INFO("Connected to WS980 at %s:%d: %s\n", m_configuration.sensorIP().c_str(), WS980_PORT,
        version.c_str());

    m_nextRead = time(nullptr);
}


vetero::common::Dataset Ws980DataReader::read()
{
    static const unsigned char get_data[] = { 0xff, 0xff, 0x0b, 0x00, 0x06, 0x04, 0x04, 0x19 };

    time_t now = time(nullptr);
    BW_DEBUG_TRACE("now = %lld, next_read=%lld", (unsigned long long)now, (unsigned long long)m_nextRead);
    while (now < m_nextRead) {
        sleep(m_nextRead - now);
        now = time(nullptr);
    }

    auto socket = connect();
    socket.write(get_data, sizeof(get_data));

    unsigned char response[256];
    ssize_t data_len = socket.read(response, sizeof(response));

    BW_DEBUG_TRACE("Got %zd bytes of data\n", data_len);

    if (data_len < 82)
        throw common::ApplicationError("ws980: response too short: got " + std::to_string(data_len) +
            " bytes instead of 82");

    common::Dataset data;
    data.setTimestamp(bw::Datetime::now());
    data.setSensorType(m_configuration.sensorType());

    int16_t temp = (response[10] << 8) | response[11];
    BW_DEBUG_TRACE("Temp raw: 0x%0hx", (uint16_t)temp);
    if (temp != 0x7fff)
        data.setTemperature(temp * 10);

    uint8_t humid = response[24];
    BW_DEBUG_TRACE("Humid raw: 0x%0hhx", humid);
    if (humid != 0xff)
        data.setHumidity(humid * 100);

    uint16_t rel_pressure = (response[29]<<8) | response[30];
    BW_DEBUG_TRACE("Pressure at sea level raw: 0x%0hx", rel_pressure);
    if (rel_pressure != 0xfff)
        data.setPressure(rel_pressure * 10);

    uint16_t wind_dir = (response[32]<<8) | response[33];
    BW_DEBUG_TRACE("Wind dir raw: 0x%0hx", wind_dir);
    if (wind_dir != 0xfff)
        data.setWindDirection(wind_dir);

    uint16_t wind_speed = ((response[35] << 8) | response[36]);
    BW_DEBUG_TRACE("Wind speed raw: 0x%0hx", wind_speed);
    if (wind_speed != 0xffff)
        data.setWindSpeed(wind_speed * 36); // 1/10 m/s -> 1/100 km/h

    uint16_t wind_gust = ((response[38] << 8) | response[39]);
    BW_DEBUG_TRACE("Wind gust raw: 0x%0hx", wind_gust);
    if (wind_gust != 0xffff)
        data.setWindGust(wind_gust * 36); // 1/10 m/s -> 1/100 km/h

    uint32_t total_rain = ((response[66] << 24) |
                           (response[67] << 16) |
                           (response[68] << 8) |
                            response[69] );
    BW_DEBUG_TRACE("Total rain raw: 0x%0x", total_rain);
    data.setRainGauge(total_rain);

    uint32_t lux = ((response[71] << 24) |
		    (response[72] << 16) |
		    (response[73] << 8) |
		    response[74] );
    BW_DEBUG_TRACE("Lux: 0x%0x", lux);
    data.setSolarRadiation(lux/10.0/126.7);

    uint16_t uv_raw = ((response[76] << 8) | response[77]);
    BW_DEBUG_TRACE("UV raw: 0x%0hx", uv_raw);

    uint8_t uv_index = response[79];
    BW_DEBUG_TRACE("UV Index Solar radiation raw: 0x%0hhx", uv_index);
    data.setUvIndex(uv_index);

    m_nextRead = now + 5*60;

    BW_DEBUG_STREAM_DBG("Read data" << data);
    return data;
}

vetero::common::DataSocket Ws980DataReader::connect()
{
    return common::DataSocket(m_configuration.sensorIP(), WS980_PORT);
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
