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
#ifndef VETERO_COMMON_DATASET_H_
#define VETERO_COMMON_DATASET_H_

#include <iostream>

#include <libbw/datetime.h>

#include "database.h"

namespace vetero {
namespace common {

/* SensorType {{{ */

class SensorType {

public:
    // Invalid sensor type, default value
    static SensorType Invalid;

    // Kombisensor which has temperature, humidity, dew point, wind and rain
    static SensorType Kombi;

    // Kombisensor which has temperature, humidity, dew point, wind, rain is present but ignored
    static SensorType KombiNoRain;

    // Pool sensor which only has temperature
    static SensorType Pool;

    // sensor with temperature and humidity
    static SensorType Normal;

    // sensor from the FreeTec station
    static SensorType FreeTec;

    // ELV WS980WiFi
    static SensorType Ws980;

public:
    static SensorType fromString(const std::string &string);

public:
    inline bool hasTemperature() const {
        return m_typeId != IdInvalid;
    }

    inline bool hasHumidity() const {
        return (m_typeId != IdInvalid) && (m_typeId != IdPool);
    }

    inline bool hasWindSpeed() const {
        return (m_typeId == IdKombi) || (m_typeId == IdKombiNoRain) || (m_typeId == IdFreeTec) || (m_typeId == IdWs980);
    }

    inline bool hasWindDirection() const {
        return (m_typeId == IdFreeTec) || (m_typeId == IdWs980);
    }

    inline bool hasWindGust() const {
        return (m_typeId == IdFreeTec) || (m_typeId == IdWs980);
    }

    inline bool hasSolarRadiation() const {
        return (m_typeId == IdWs980);
    }

    inline bool hasRain() const {
        return (m_typeId == IdKombi) || (m_typeId == IdFreeTec) || (m_typeId == IdWs980);
    }

    inline bool hasPressure() const {
        return (m_typeId == IdFreeTec) || (m_typeId == IdWs980);
    }

    bool operator==(const SensorType &other) const {
        return m_typeId == other.m_typeId;
    }

    bool operator!=(const SensorType &other) const {
        return m_typeId != other.m_typeId;
    }

    std::string str() const;

private:
    // the first IDs are for the USB WDE-01 from ELV, the IdFreeTec
    // is the FreeTec station from Pearl
    enum TypeId {
        IdInvalid = -1, IdKombi, IdKombiNoRain, IdPool, IdNormal, IdFreeTec, IdWs980
    };
    SensorType(TypeId type) : m_typeId(type) {}


private:
    TypeId m_typeId;
};

std::ostream &operator<<(std::ostream &os, const SensorType &type);

/* }}} */
/* Dataset {{{ */

//
// \class Dataset
// \brief Dataset as received by the weather station.
//
// This contains class now only implements the Kombisensor. Future versions \em may include more
// receivers.
//
class Dataset {

    public:
        SensorType sensorType() const;
        void setSensorType(const SensorType &type);

        bw::Datetime timestamp() const;
        void setTimestamp(const bw::Datetime &timestamp);

        int temperature() const;
        void setTemperature(int temperature);

        int humidity() const;
        void setHumidity(int humidity);

        int windSpeed() const;
        void setWindSpeed(int windSpeed);

        int windGust() const;
        void setWindGust(int windGust);

        int windDirection() const;
        void setWindDirection(int windDirection);

        int pressure() const;
        void setPressure(int pressure);

        int solarRadiation() const;
        void setSolarRadiation(int radiation);

        int uvIndex() const;
        void setUvIndex(int index);

        int rainGauge() const;
        void setRainGauge(int rainGauge);

        int rainGaugeFactor() const;

        bool isRain() const;
        void setIsRain(bool rain);

        std::string str() const;

    private:
        SensorType m_sensorType = SensorType::Invalid;
        bw::Datetime m_timestamp;
        int m_temperature = 0;
        int m_humidity = 0;
        int m_windSpeed = 0;
        int m_windGust = 0;
        int m_pressure = 0; // 1/100 hPa, sea level
        int m_rainGauge = 0;
        int m_solarRadiation = 0; // W/m^2
        int m_uvIndex = 0;
        bool m_IsRain = false;
        int m_windDirection = 0; // degrees
};

std::ostream &operator<<(std::ostream &os, const Dataset &dataset);

/* }}} */
/* CurrentWeather {{{ */

// \class CurrentWeather
// \brief Current weather data
//
// This is a container class for the current weather. The difference to the Dataset is that
// this class contains the accumulated rain amount of a day and the min/max/avg values of the day.
//
class CurrentWeather
{
    public:
        virtual ~CurrentWeather() {}

    public:
        bw::Datetime timestamp() const;
        void setTimestamp(const bw::Datetime &timestamp);

        // Temperature

        int temperature() const;
        double temperatureReal() const;
        double temperatureRealF() const;
        void setTemperature(int temperature);

        int minTemperature() const;
        double minTemperatureReal() const;
        void setMinTemperature(int minTemperature);

        int maxTemperature() const;
        double maxTemperatureReal() const;
        void setMaxTemperature(int maxTemperature);

        // Humidity and Dewpoint

        bool hasHumidity() const;

        int humidity() const;
        double humidityReal() const;
        void setHumidity(int humidity);

        int dewpoint() const;
        double dewpointReal() const;
        double dewpointRealF() const;
        void setDewpoint(int dewpoint);

        // Pressure

        bool hasPressure() const;
        int pressure() const;
        double pressureReal() const;
        double pressureRealIn() const;
        void setPressure(int pressure);

        // Wind

        bool hasWindSpeed() const;

        int windSpeed() const;
        double windSpeedReal() const;
        double windSpeedRealMph() const;
        void setWindSpeed(int windSpeed);

        int windBeaufort() const;
        void setWindBeaufort(int bft);

        int maxWindSpeed() const;
        double maxWindSpeedReal() const;
        void setMaxWindSpeed(int windSpeed);

        int maxWindBeaufort() const;
        void setMaxWindBeaufort(int bft);

        // Wind Gust

        bool hasWindGust() const;

        int windGust() const;
        double windGustReal() const;
        double windGustRealMph() const;
        void setWindGust(int windGust);

        int windGustBeaufort() const;
        void setWindGustBeaufort(int gustBft);

        int maxWindGust() const;
        double maxWindGustReal() const;
        void setMaxWindGust(int windSpeed);

        int maxWindGustBeaufort() const;
        void setMaxGustWindBeaufort(int bft);

        // Wind Direction

        bool hasWindDirection() const;
        int windDirection() const;
        void setWindDirection(int windDirection);
        std::string windDirectionStr() const;

        // Solar Radiation

        bool hasSolarRadiation() const;
        int solarRadiation() const;
        void setSolarRadiation(int radiation);

        int uvIndex() const;
        void setUvIndex(int index);

        // Rain

        bool hasRain() const;
        int rain() const;
        double rainReal() const;
        double rainRealIn() const;
        void setRain(int  rain);

        /** \} */                                                                          /* }}} */

        /**
         * \brief Creates a string-represenation of the object
         *
         * This saves us a friend declaration and also makes it easier to use C-like debugging
         * frameworks.
         *
         * \return the string representation for debugging
         */
        std::string str() const;

    private:
        bw::Datetime m_timestamp;
        int m_temperature = 0, m_minTemperature = 0, m_maxTemperature = 0;

        bool m_hasHumidity = false;
        int m_humidity = 0, m_dewpoint = 0;

        bool m_hasPressure = false;
        int m_pressure = 0;

        bool m_hasWindSpeed = false;
        int m_windSpeed = 0, m_maxWindSpeed = 0, m_windBft = 0;

        bool m_hasWindGust = false;
        int m_windGust = 0, m_maxWindGust = 0, m_windGustBft = 0;

        bool m_hasWindDirection = false;
        int m_windDirection = 0; // degrees

        bool m_hasSolarRadiation = false;
        int m_solarRadiation = 0, m_uvIndex = 0;

        bool m_hasRain = false;
        int m_rain = 0;

};

/**
 * \brief Prints a CurrentWeather object
 *
 * \param[in] os the output stream
 * \param[in] dataset the data set object
 * \return the output stream
 */
std::ostream &operator<<(std::ostream &os, const CurrentWeather &dataset);

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // VETERO_COMMON_DATASET_H_
