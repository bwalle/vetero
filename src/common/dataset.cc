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
#include <iostream>
#include <sstream>

#include "dataset.h"
#include "utils.h"
#include "weather.h"

namespace vetero {
namespace common {

/* UsbWde1Dataset {{{ */

// -------------------------------------------------------------------------------------------------
UsbWde1Dataset::UsbWde1Dataset()
    : m_sensorType(SensorInvalid)
    , m_temperature(0)
    , m_humidity(0)
    , m_windSpeed(0)
    , m_rainGauge(0)
    , m_IsRain(false)
{}

// -------------------------------------------------------------------------------------------------
UsbWde1Dataset::SensorType UsbWde1Dataset::sensorType() const
{
    return m_sensorType;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setSensorType(UsbWde1Dataset::SensorType type)
{
    m_sensorType = type;
}

// -------------------------------------------------------------------------------------------------
bw::Datetime UsbWde1Dataset::timestamp() const
{
    return m_timestamp;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setTimestamp(const bw::Datetime &time)
{
    m_timestamp = time;
}

// -------------------------------------------------------------------------------------------------
int UsbWde1Dataset::temperature() const
{
    return m_temperature;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setTemperature(int temperature)
{
    m_temperature = temperature;
}

// -------------------------------------------------------------------------------------------------
int UsbWde1Dataset::humidity() const
{
    return m_humidity;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setHumidity(int humidity)
{
    m_humidity = humidity;
}

// -------------------------------------------------------------------------------------------------
int UsbWde1Dataset::windSpeed() const
{
    return m_windSpeed;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setWindSpeed(int windSpeed)
{
    m_windSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
int UsbWde1Dataset::rainGauge() const
{
    return m_rainGauge;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setRainGauge(int rainGauge)
{
    m_rainGauge = rainGauge;
}

// -------------------------------------------------------------------------------------------------
bool UsbWde1Dataset::isRain() const
{
    return m_IsRain;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setIsRain(bool rain)
{
    m_IsRain = rain;
}

// -------------------------------------------------------------------------------------------------
std::string UsbWde1Dataset::str() const
{
    std::stringstream ss;
    ss << "time="       << timestamp() << ", "
       << "temp="       << temperature() << "C, "
       << "humid="      << humidity() << "%, "
       << "wind="       << windSpeed() << "km/h, "
       << "rainGauge="  << rainGauge() << ", "
       << "rain="       << std::boolalpha << isRain();
    return ss.str();
}

// -------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const UsbWde1Dataset &dataset)
{
    os << dataset.str();
    return os;
}

/* }}} */
/* CurrentWeather {{{ */

// -------------------------------------------------------------------------------------------------
CurrentWeather::CurrentWeather()
    : m_temperature(0)
    , m_minTemperature(0)
    , m_maxTemperature(0)
    , m_humidity(0)
    , m_dewpoint(0)
    , m_windSpeed(0)
    , m_maxWindSpeed(0)
    , m_windBft(0)
    , m_maxWindBft(0)
    , m_rain(0)
{}

// -------------------------------------------------------------------------------------------------
bw::Datetime CurrentWeather::timestamp() const
{
    return m_timestamp;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setTimestamp(const bw::Datetime &time)
{
    m_timestamp = time;
}

/* Temperature {{{ */

// -------------------------------------------------------------------------------------------------
int CurrentWeather::temperature() const
{
    return m_temperature;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::temperatureReal() const
{
    return m_temperature/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setTemperature(int temperature)
{
    m_temperature = temperature;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::minTemperature() const
{
    return m_minTemperature;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::minTemperatureReal() const
{
    return m_minTemperature/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMinTemperature(int minTemperature)
{
    m_minTemperature = minTemperature;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::maxTemperature() const
{
    return m_maxTemperature;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::maxTemperatureReal() const
{
    return m_maxTemperature/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMaxTemperature(int maxTemperature)
{
    m_maxTemperature = maxTemperature;
}

/* }}} */
/* Humidity and Dewpoint {{{ */

// -------------------------------------------------------------------------------------------------
int CurrentWeather::humidity() const
{
    return m_humidity;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::humidityReal() const
{
    return m_humidity/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setHumidity(int humidity)
{
    m_humidity = humidity;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::dewpoint() const
{
    return m_dewpoint;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::dewpointReal() const
{
    return m_dewpoint/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setDewpoint(int dewpoint)
{
    m_dewpoint = dewpoint;
}

/* }}} */
/* Wind {{{ */

// -------------------------------------------------------------------------------------------------
int CurrentWeather::windSpeed() const
{
    return m_windSpeed;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::windSpeedReal() const
{
    return m_windSpeed/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setWindSpeed(int windSpeed)
{
    m_windSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::windBeaufort() const
{
    return m_windBft;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setWindBeaufort(int bft)
{
    m_windBft = bft;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::maxWindSpeed() const
{
    return m_maxWindSpeed;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::maxWindSpeedReal() const
{
    return m_maxWindSpeed/100.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMaxWindSpeed(int windSpeed)
{
    m_maxWindSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::maxWindBeaufort() const
{
    return Weather::windSpeedToBft(m_maxWindSpeed);
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMaxWindBeaufort(int bft)
{
    m_maxWindBft = bft;
}

/* }}} */
/* Rain {{{ */

// -------------------------------------------------------------------------------------------------
int CurrentWeather::rain() const
{
    return m_rain;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::rainReal() const
{
    return m_rain/1000.0;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setRain(int rain)
{
    m_rain = rain;
}

/* }}} */

// -------------------------------------------------------------------------------------------------
std::string CurrentWeather::str() const
{
    std::stringstream ss;
    ss << "temperature="        << temperature() << "C, "
       << "minTemperature="     << minTemperature() << "C, "
       << "maxTemperature="     << maxTemperature() << "C, "
       << "humidity="           << humidity() << "%, "
       << "dewpoint="           << dewpoint() << "C, "
       << "windSpeed="          << windSpeed() << ", "
       << "windSpeed="          << windBeaufort() << " Bft, "
       << "maxWindSpeed="       << maxWindSpeed() << ", "
       << "maxWindSpeed="       << maxWindBeaufort() << " Bft, "
       << "rain="               << rain() << ", ";
    return ss.str();
}

// -------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const CurrentWeather &dataset)
{
    os << dataset.str();
    return os;
}

/* }}} */

} // end namespace common
} // end namespace vetero
