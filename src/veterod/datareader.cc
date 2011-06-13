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
#include <libbw/stringutil.h>
#include <libbw/log/debug.h>
#include <libbw/datetime.h>

#include "datareader.h"

/**
 * @file
 * @brief Contains the weather data reader (serial device)
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup daemon
 */

namespace vetero {
namespace daemon {

/* DataReader {{{ */

// -------------------------------------------------------------------------------------------------
DataReader::DataReader(const std::string &deviceName, int baud)
    : m_serialDeviceName(deviceName)
    , m_serialDevice(m_serialDeviceName)
    , m_baud(baud)
{}

// -------------------------------------------------------------------------------------------------
void DataReader::openConnection()
    throw (common::ApplicationError)
{
    if (!m_serialDevice.openPort())
        throw common::ApplicationError("Unable to open port '"+ m_serialDeviceName + "': " +
                               m_serialDevice.getLastError());

    if (!m_serialDevice.reconfigure(m_baud, bw::io::SerialFile::FC_NONE))
        throw common::ApplicationError("Unable to configure serial port: " + m_serialDevice.getLastError());

    BW_DEBUG_INFO("Connection to serial port etablished.");
}

// -------------------------------------------------------------------------------------------------
vetero::common::UsbWde1Dataset DataReader::read()
    throw (common::ApplicationError)
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

    vetero::common::UsbWde1Dataset data = parseDataset(line);
    data.setTimestamp(bw::Datetime::now());

    BW_DEBUG_DBG("Read dataset: %s", data.str().c_str());

    return data;
}

// -------------------------------------------------------------------------------------------------
vetero::common::UsbWde1Dataset DataReader::parseDataset(const std::string &line) const
    throw (common::ApplicationError)
{
    const int TEMPERATURE_INDEX = 19;
    const int HUMIDITY_INDEX    = 20;
    const int WIND_INDEX        = 21;
    const int RAIN_INDEX        = 22;
    const int IS_RAIN_INDEX     = 23;

    std::vector<std::string> parts = bw::stringsplit(line, ";");
    if (parts.size() != 25)
        throw common::ApplicationError("Invalid data set received: " + bw::str(parts.size()) +
                               " instead of 25 parts.");

    vetero::common::UsbWde1Dataset data;

    // temperature
    std::string temperature = parts[TEMPERATURE_INDEX];
    temperature = bw::replace_char(temperature, ',', ".");
    data.setTemperature( bw::from_str<double>(temperature) );

    // humidity
    std::string humidity = parts[HUMIDITY_INDEX];
    data.setHumidity( bw::from_str<int>(humidity) );

    // wind
    std::string wind = parts[WIND_INDEX];
    wind = bw::replace_char(wind, ',', ".");
    data.setWindSpeed( bw::from_str<double>(wind) );

    // rain
    std::string rain = parts[RAIN_INDEX];
    data.setRainGauge( bw::from_str<int>(rain) );

    // is raining
    std::string isRain = parts[IS_RAIN_INDEX];
    data.setIsRain( bw::from_str<int>(isRain) );

    return data;
}

/* }}} */

} // end namespace vetero
} // end namespace daemon
