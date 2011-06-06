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
#include <limits>
#include <iostream>
#include <sstream>
#include <cmath>

#include "dataset.h"
#include "utils.h"
#include "weather.h"

namespace vetero {
namespace common {

/* UsbWde1Dataset {{{ */

// -------------------------------------------------------------------------------------------------
UsbWde1Dataset::UsbWde1Dataset()
    : m_temperature(std::numeric_limits<double>::min())
    , m_humidity(std::numeric_limits<int>::min())
    , m_dewpoint(std::numeric_limits<double>::min())
    , m_windSpeed(std::numeric_limits<double>::min())
    , m_rainGauge(std::numeric_limits<unsigned long long>::max())
    , m_IsRain(false)
{}

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
double UsbWde1Dataset::temperature() const
{
    return m_temperature;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setTemperature(double temperature)
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
double UsbWde1Dataset::dewpoint() const
{
    return m_dewpoint;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setDewpoint(double dewpoint)
{
    m_dewpoint = dewpoint;
}

// -------------------------------------------------------------------------------------------------
double UsbWde1Dataset::windSpeed() const
{
    return m_windSpeed;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setWindSpeed(double windSpeed)
{
    m_windSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
unsigned long long UsbWde1Dataset::rainGauge() const
{
    return m_rainGauge;
}

// -------------------------------------------------------------------------------------------------
void UsbWde1Dataset::setRainGauge(unsigned long long rainGauge)
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
       << "dewpoint="   << dewpoint() << "C, "
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
    : m_temperature(std::numeric_limits<double>::min())
    , m_minTemperature(std::numeric_limits<double>::min())
    , m_maxTemperature(std::numeric_limits<double>::min())
    , m_humidity(std::numeric_limits<int>::min())
    , m_dewpoint(std::numeric_limits<double>::min())
    , m_windSpeed(std::numeric_limits<double>::min())
    , m_maxWindSpeed(std::numeric_limits<double>::min())
    , m_rain(std::numeric_limits<double>::min())
    , m_IsRain(false)
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

// -------------------------------------------------------------------------------------------------
double CurrentWeather::temperature() const
{
    return m_temperature;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setTemperature(double temperature)
{
    m_temperature = temperature;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::minTemperature() const
{
    return m_minTemperature;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMinTemperature(double minTemperature)
{
    m_minTemperature = minTemperature;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::maxTemperature() const
{
    return m_maxTemperature;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMaxTemperature(double maxTemperature)
{
    m_maxTemperature = maxTemperature;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::humidity() const
{
    return m_humidity;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setHumidity(int humidity)
{
    m_humidity = humidity;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::dewpoint() const
{
    return m_dewpoint;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setDewpoint(double dewpoint)
{
    m_dewpoint = dewpoint;
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::windSpeed() const
{
    return m_windSpeed;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setWindSpeed(double windSpeed)
{
    m_windSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::windBeaufort() const
{
    return Weather::windSpeedToBft(m_windSpeed);
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::maxWindSpeed() const
{
    return m_maxWindSpeed;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setMaxWindSpeed(double windSpeed)
{
    m_maxWindSpeed = windSpeed;
}

// -------------------------------------------------------------------------------------------------
int CurrentWeather::maxWindBeaufort() const
{
    return Weather::windSpeedToBft(m_maxWindSpeed);
}

// -------------------------------------------------------------------------------------------------
double CurrentWeather::rain() const
{
    return m_rain;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setRain(double rain)
{
    m_rain = rain;
}

// -------------------------------------------------------------------------------------------------
bool CurrentWeather::isRain() const
{
    return m_IsRain;
}

// -------------------------------------------------------------------------------------------------
void CurrentWeather::setIsRain(bool rain)
{
    m_IsRain = rain;
}

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
       << "rain="               << rain() << ", "
       << "isRain="             << std::boolalpha << isRain();
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
