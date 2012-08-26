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

/* UsbWde1Dataset {{{ */

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

public:
    static SensorType fromString(const std::string &string);

public:
    inline bool hasTemperature() const {
        return m_typeId != IdInvalid;
    }

    inline bool hasHumidity() const {
        return (m_typeId != IdInvalid) && (m_typeId != IdPool);
    }

    inline bool hasWind() const {
        return (m_typeId == IdKombi) || (m_typeId == IdKombiNoRain);
    }

    inline bool hasRain() const {
        return m_typeId == IdKombi;
    }

    bool operator==(const SensorType &other) const {
        return m_typeId == other.m_typeId;
    }

    bool operator!=(const SensorType &other) const {
        return m_typeId != other.m_typeId;
    }

    std::string str() const;

private:
    enum TypeId {
        IdInvalid = -1, IdKombi, IdKombiNoRain, IdPool, IdNormal
    };
    SensorType(TypeId type) : m_typeId(type) {}


private:
    TypeId m_typeId;
};

std::ostream &operator<<(std::ostream &os, const SensorType &type);

//
// \class UsbWde1Dataset
// \brief Dataset as received by the USB-WDE1 weather station.
//
// This contains class now only implements the Kombisensor. Future versions \em may include more
// receivers.
//
class UsbWde1Dataset {

    public:
        // The amount of a rain gauge in 1/1000 mm.
        static const int RAIN_GAUGE_FACTOR = 295;

    public:
        UsbWde1Dataset();
        virtual ~UsbWde1Dataset() {}

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

        int rainGauge() const;
        void setRainGauge(int rainGauge);

        bool isRain() const;
        void setIsRain(bool rain);

        std::string str() const;

    private:
        SensorType m_sensorType;
        bw::Datetime m_timestamp;
        int m_temperature;
        int m_humidity;
        int m_windSpeed;
        int m_rainGauge;
        bool m_IsRain;
};

std::ostream &operator<<(std::ostream &os, const UsbWde1Dataset &dataset);

/* }}} */
/* CurrentWeather {{{ */

// \class CurrentWeather
// \brief Current weather data
//
// This is a container class for the current weather. The difference to the UsbWde1Dataset is that
// this class contains the accumulated rain amount of a day and the min/max/avg values of the day.
//
class CurrentWeather
{
    public:
        CurrentWeather();
        virtual ~CurrentWeather() {}

    public:
        bw::Datetime timestamp() const;
        void setTimestamp(const bw::Datetime &timestamp);

        // Temperature

        int temperature() const;
        double temperatureReal() const;
        void setTemperature(int temperature);

        int minTemperature() const;
        double minTemperatureReal() const;
        void setMinTemperature(int minTemperature);

        int maxTemperature() const;
        double maxTemperatureReal() const;
        void setMaxTemperature(int maxTemperature);

        // Humidity and Dewpoint

        int humidity() const;
        double humidityReal() const;
        void setHumidity(int humidity);

        int dewpoint() const;
        double dewpointReal() const;
        void setDewpoint(int dewpoint);

        // Pressure

        int pressure() const;
        double pressureReal() const;
        void setPressure(int pressure);

        // Wind

        int windSpeed() const;
        double windSpeedReal() const;
        void setWindSpeed(int windSpeed);

        int windBeaufort() const;
        void setWindBeaufort(int bft);

        int maxWindSpeed() const;
        double maxWindSpeedReal() const;
        void setMaxWindSpeed(int windSpeed);

        int maxWindBeaufort() const;
        void setMaxWindBeaufort(int bft);

        // Rain

        int rain() const;
        double rainReal() const;
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
        int m_temperature, m_minTemperature, m_maxTemperature;
        int m_humidity, m_dewpoint;
        int m_pressure;
        int m_windSpeed, m_maxWindSpeed, m_windBft, m_maxWindBft;
        int m_rain;
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
