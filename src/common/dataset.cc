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
SensorType SensorType::FreeTec(IdFreeTec);
SensorType SensorType::Ws980(IdWs980);

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

        case IdFreeTec:
            return "freetec";

        case IdWs980:
            return "ws980";

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
    else if (strcasecmp(value.c_str(), "freetec") == 0)
        return FreeTec;
    else if (strcasecmp(value.c_str(), "ws980") == 0)
        return Ws980;
    else
        return Invalid;
}

std::ostream &operator<<(std::ostream &os, const SensorType &type)
{
    return os << type.str();
}

/* }}} */
/* Dataset {{{ */

SensorType Dataset::sensorType() const
{
    return m_sensorType;
}

void Dataset::setSensorType(const SensorType &type)
{
    m_sensorType = type;
}

bw::Datetime Dataset::timestamp() const
{
    return m_timestamp;
}

void Dataset::setTimestamp(const bw::Datetime &time)
{
    m_timestamp = time;
}

int Dataset::temperature() const
{
    return m_temperature;
}

void Dataset::setTemperature(int temperature)
{
    m_temperature = temperature;
}

int Dataset::humidity() const
{
    return m_humidity;
}

void Dataset::setHumidity(int humidity)
{
    m_humidity = humidity;
}

int Dataset::windSpeed() const
{
    return m_windSpeed;
}

void Dataset::setWindSpeed(int windSpeed)
{
    m_windSpeed = windSpeed;
}

int Dataset::windGust() const
{
    return m_windGust;
}

void Dataset::setWindGust(int windGust)
{
    m_windGust = windGust;
}

int Dataset::windDirection() const
{
    return m_windDirection;
}

int Dataset::pressure() const
{
    return m_pressure;
}

void Dataset::setPressure(int pressure)
{
    m_pressure = pressure;
}

void Dataset::setWindDirection(int windDirection)
{
    m_windDirection = windDirection;
}

int Dataset::rainGauge() const
{
    return m_rainGauge;
}

void Dataset::setRainGauge(int rainGauge)
{
    m_rainGauge = rainGauge;
}

bool Dataset::isRain() const
{
    return m_IsRain;
}

void Dataset::setIsRain(bool rain)
{
    m_IsRain = rain;
}

int Dataset::rainGaugeFactor() const
{
    if (m_sensorType == SensorType::FreeTec)
        return 300;
    else if (m_sensorType == SensorType::Ws980)
        return 100;
    else
        return 295;
}

std::string Dataset::str() const
{
    std::stringstream ss;
    ss << "time="             << timestamp() << ", "
       << "temp="             << temperature() << "C, "
       << "humid="            << humidity() << "%, "
       << "wind="             << windSpeed() << "km/h, "
       << "wind="             << windGust() << "km/h, "
       << "windDirection="    << windDirection() << "deg, "
       << "pressure="         << pressure() << "hPa, "
       << "rainGauge="        << rainGauge() << ", "
       << "rain="             << std::boolalpha << isRain();
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const Dataset &dataset)
{
    os << dataset.str();
    return os;
}

/* }}} */
/* CurrentWeather {{{ */

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

double CurrentWeather::temperatureRealF() const
{
    return weather::celsius2Fahrenheit( temperatureReal() );
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

double CurrentWeather::dewpointRealF() const
{
    return weather::celsius2Fahrenheit( dewpointReal() );
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

double CurrentWeather::pressureRealIn() const
{
    return weather::hPa2inHg( pressureReal() );
}

void CurrentWeather::setPressure(int pressure)
{
    m_hasPressure = true;
    m_pressure = pressure;
}

/* }}} */
/* Wind {{{ */

bool CurrentWeather::hasWindSpeed() const
{
    return m_hasWindSpeed;
}

int CurrentWeather::windSpeed() const
{
    return m_windSpeed;
}

double CurrentWeather::windSpeedReal() const
{
    return m_windSpeed/100.0;
}

double CurrentWeather::windSpeedRealMph() const
{
    return weather::kmh2mph( windSpeedReal() );
}

void CurrentWeather::setWindSpeed(int windSpeed)
{
    m_hasWindSpeed = true;
    m_windSpeed = windSpeed;
}

int CurrentWeather::windBeaufort() const
{
    return m_windBft;
}

void CurrentWeather::setWindBeaufort(int bft)
{
    m_hasWindSpeed = true;
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
    return weather::windSpeedToBft(m_maxWindSpeed);
}

/* }}} */
/* Wind Gust {{{ */

bool CurrentWeather::hasWindGust() const
{
    return m_hasWindGust;
}

int CurrentWeather::windGust() const
{
    return m_windGust;
}

double CurrentWeather::windGustReal() const
{
    return m_windGust/100.0;
}

double CurrentWeather::windGustRealMph() const
{
    return weather::kmh2mph( windGustReal() );
}

void CurrentWeather::setWindGust(int windSpeed)
{
    m_hasWindGust = true;
    m_windGust = windSpeed;
}

int CurrentWeather::windGustBeaufort() const
{
    return m_windGustBft;
}

void CurrentWeather::setWindGustBeaufort(int bft)
{
    m_hasWindGust = true;
    m_windGustBft = bft;
}

int CurrentWeather::maxWindGust() const
{
    return m_maxWindGust;
}

double CurrentWeather::maxWindGustReal() const
{
    return m_maxWindGust/100.0;
}

void CurrentWeather::setMaxWindGust(int windSpeed)
{
    m_maxWindGust = windSpeed;
}

int CurrentWeather::maxWindGustBeaufort() const
{
    return weather::windSpeedToBft(m_maxWindGust);
}

/* }}} */
/* Wind Direction {{{ */

bool CurrentWeather::hasWindDirection() const
{
    return m_hasWindDirection;
}

int CurrentWeather::windDirection() const
{
    return m_windDirection;
}

void CurrentWeather::setWindDirection(int windDirection)
{
    m_hasWindDirection = true;
    m_windDirection = windDirection;
}

std::string CurrentWeather::windDirectionStr() const
{
    const char *windDirs[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
	"S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW",
    };

    int index = m_windDirection/22 % 16;
    return windDirs[index];
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

double CurrentWeather::rainRealIn() const
{
    return weather::mm2in( rainReal() );
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

    if (hasWindSpeed()) {
        ss << "windSpeed=" << windSpeedReal() << ", "
           << "windSpeed=" << windBeaufort() << " Bft, "
           << "maxWindSpeed=" << maxWindSpeedReal() << ", "
           << "maxWindSpeed=" << maxWindBeaufort() << " Bft, ";
    }

    if (hasWindGust()) {
        ss << "windGust=" << windGustReal() << ", "
           << "windGust=" << windGustBeaufort() << ", "
           << "maxWindGust=" << maxWindGust() << ", "
           << "maxWindGust=" << maxWindGustBeaufort() << "Bft, ";
    }

    if (hasWindDirection())
        ss << "windDirection=" << windDirection() << " deg, ";

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
