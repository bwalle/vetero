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
#include <iostream>
#include <sstream>

#include "dataset.h"
#include "utils.h"
#include "weather.h"

namespace vetero {
namespace common {

/* SensorType {{{ */

SensorType SensorType::Invalid(IdInvalid);
SensorType SensorType::Kombi(IdKombi);
SensorType SensorType::KombiNoRain(IdKombiNoRain);
SensorType SensorType::Pool(IdPool);
SensorType SensorType::Normal(IdNormal);

std::string SensorType::str() const
{
    switch (m_typeId) {
        case IdKombi:
            return "kombi";

        case IdKombiNoRain:
            return "kombi_no_rain";

        case IdPool:
            return "pool";

        case IdNormal:
            return "normal";

        case IdInvalid:
        default:
            return "invalid";
    }
}

SensorType SensorType::fromString(const std::string &value)
{
    if (strcasecmp(value.c_str(), "kombi") == 0)
        return Kombi;
    else if (strcasecmp(value.c_str(), "kombi_no_rain") == 0)
        return KombiNoRain;
    else if (strcasecmp(value.c_str(), "pool") == 0)
        return Pool;
    else if (strcasecmp(value.c_str(), "normal") == 0)
        return Normal;
    else
        return Invalid;
}

std::ostream &operator<<(std::ostream &os, const SensorType &type)
{
    return os << type.str();
}

/* }}} */
/* UsbWde1Dataset {{{ */

UsbWde1Dataset::UsbWde1Dataset()
    : m_sensorType(SensorType::Invalid)
    , m_temperature(0)
    , m_humidity(0)
    , m_windSpeed(0)
    , m_rainGauge(0)
    , m_IsRain(false)
{}

SensorType UsbWde1Dataset::sensorType() const
{
    return m_sensorType;
}

void UsbWde1Dataset::setSensorType(const SensorType &type)
{
    m_sensorType = type;
}

bw::Datetime UsbWde1Dataset::timestamp() const
{
    return m_timestamp;
}

void UsbWde1Dataset::setTimestamp(const bw::Datetime &time)
{
    m_timestamp = time;
}

int UsbWde1Dataset::temperature() const
{
    return m_temperature;
}

void UsbWde1Dataset::setTemperature(int temperature)
{
    m_temperature = temperature;
}

int UsbWde1Dataset::humidity() const
{
    return m_humidity;
}

void UsbWde1Dataset::setHumidity(int humidity)
{
    m_humidity = humidity;
}

int UsbWde1Dataset::windSpeed() const
{
    return m_windSpeed;
}

void UsbWde1Dataset::setWindSpeed(int windSpeed)
{
    m_windSpeed = windSpeed;
}

int UsbWde1Dataset::rainGauge() const
{
    return m_rainGauge;
}

void UsbWde1Dataset::setRainGauge(int rainGauge)
{
    m_rainGauge = rainGauge;
}

bool UsbWde1Dataset::isRain() const
{
    return m_IsRain;
}

void UsbWde1Dataset::setIsRain(bool rain)
{
    m_IsRain = rain;
}

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

std::ostream &operator<<(std::ostream &os, const UsbWde1Dataset &dataset)
{
    os << dataset.str();
    return os;
}

/* }}} */
/* CurrentWeather {{{ */

CurrentWeather::CurrentWeather()
    : m_temperature(0),
      m_minTemperature(0),
      m_maxTemperature(0),

      m_hasHumidity(false),
      m_humidity(0),
      m_dewpoint(0),

      m_hasPressure(false),
      m_pressure(0),

      m_hasWind(false),
      m_windSpeed(0),
      m_maxWindSpeed(0),
      m_windBft(0),
      m_maxWindBft(0),

      m_hasRain(false),
      m_rain(0)
{}

bw::Datetime CurrentWeather::timestamp() const
{
    return m_timestamp;
}

void CurrentWeather::setTimestamp(const bw::Datetime &time)
{
    m_timestamp = time;
}

/* Temperature {{{ */

int CurrentWeather::temperature() const
{
    return m_temperature;
}

double CurrentWeather::temperatureReal() const
{
    return m_temperature/100.0;
}

void CurrentWeather::setTemperature(int temperature)
{
    m_temperature = temperature;
}

int CurrentWeather::minTemperature() const
{
    return m_minTemperature;
}

double CurrentWeather::minTemperatureReal() const
{
    return m_minTemperature/100.0;
}

void CurrentWeather::setMinTemperature(int minTemperature)
{
    m_minTemperature = minTemperature;
}

int CurrentWeather::maxTemperature() const
{
    return m_maxTemperature;
}

double CurrentWeather::maxTemperatureReal() const
{
    return m_maxTemperature/100.0;
}

void CurrentWeather::setMaxTemperature(int maxTemperature)
{
    m_maxTemperature = maxTemperature;
}

/* }}} */
/* Humidity and Dewpoint {{{ */

bool CurrentWeather::hasHumidity() const
{
    return m_hasHumidity;
}

int CurrentWeather::humidity() const
{
    return m_humidity;
}

double CurrentWeather::humidityReal() const
{
    return m_humidity/100.0;
}

void CurrentWeather::setHumidity(int humidity)
{
    m_hasHumidity = true;
    m_humidity = humidity;
}

int CurrentWeather::dewpoint() const
{
    return m_dewpoint;
}

double CurrentWeather::dewpointReal() const
{
    return m_dewpoint/100.0;
}

void CurrentWeather::setDewpoint(int dewpoint)
{
    m_dewpoint = dewpoint;
}

/* }}} */
/* Pressure {{{ */

bool CurrentWeather::hasPressure() const
{
    return m_hasPressure;
}

int CurrentWeather::pressure() const
{
    return m_pressure;
}

double CurrentWeather::pressureReal() const
{
    return m_pressure/100.0;
}

void CurrentWeather::setPressure(int pressure)
{
    m_hasPressure = true;
    m_pressure = pressure;
}

/* }}} */
/* Wind {{{ */

bool CurrentWeather::hasWind() const
{
    return m_hasWind;
}

int CurrentWeather::windSpeed() const
{
    return m_windSpeed;
}

double CurrentWeather::windSpeedReal() const
{
    return m_windSpeed/100.0;
}

void CurrentWeather::setWindSpeed(int windSpeed)
{
    m_hasWind = true;
    m_windSpeed = windSpeed;
}

int CurrentWeather::windBeaufort() const
{
    return m_windBft;
}

void CurrentWeather::setWindBeaufort(int bft)
{
    m_hasWind = true;
    m_windBft = bft;
}

int CurrentWeather::maxWindSpeed() const
{
    return m_maxWindSpeed;
}

double CurrentWeather::maxWindSpeedReal() const
{
    return m_maxWindSpeed/100.0;
}

void CurrentWeather::setMaxWindSpeed(int windSpeed)
{
    m_maxWindSpeed = windSpeed;
}

int CurrentWeather::maxWindBeaufort() const
{
    return Weather::windSpeedToBft(m_maxWindSpeed);
}

void CurrentWeather::setMaxWindBeaufort(int bft)
{
    m_maxWindBft = bft;
}

/* }}} */
/* Rain {{{ */

bool CurrentWeather::hasRain() const
{
    return m_hasRain;
}

int CurrentWeather::rain() const
{
    return m_rain;
}

double CurrentWeather::rainReal() const
{
    return m_rain/1000.0;
}

void CurrentWeather::setRain(int rain)
{
    m_hasRain = true;
    m_rain = rain;
}

/* }}} */

std::string CurrentWeather::str() const
{
    std::stringstream ss;
    ss << "temperature="        << temperatureReal() << "C, "
       << "minTemperature="     << minTemperatureReal() << "C, "
       << "maxTemperature="     << maxTemperatureReal() << "C, ";

    if (hasHumidity()) {
       ss << "humidity=" << humidityReal() << "%, "
          << "dewpoint=" << dewpointReal() << "C, ";
    }

    if (hasPressure())
        ss << "pressure=" << hasPressure() << "hPa, ";

    if (hasWind()) {
        ss << "windSpeed=" << windSpeedReal() << ", "
           << "windSpeed=" << windBeaufort() << " Bft, "
           << "maxWindSpeed=" << maxWindSpeedReal() << ", "
           << "maxWindSpeed=" << maxWindBeaufort() << " Bft, ";
    }

    if (hasRain())
        ss << "rain=" << rainReal() << ", ";

    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const CurrentWeather &dataset)
{
    os << dataset.str();
    return os;
}

/* }}} */

} // end namespace common
} // end namespace vetero
