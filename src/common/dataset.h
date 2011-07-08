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
#ifndef DATASET_H
#define DATASET_H

#include <iostream>

#include <libbw/datetime.h>

#include "database.h"

namespace vetero {
namespace common {

/* UsbWde1Dataset {{{ */

/**
 * @class UsbWde1Dataset
 * @brief Dataset as received by the USB-WDE1 weather station.
 *
 * This contains class now only implements the Kombisensor. Future versions \em may include more
 * receivers.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */
class UsbWde1Dataset {

    public:
        /**
         * @brief The amount of a rain gauge in 1/1000 mm.
         */
        static const int RAIN_GAUGE_FACTOR = 295;

        /**
         * @brief Sensor type, comes from the user configuration
         */
        enum SensorType {
            SensorInvalid = -1, /**< Invalid sensor type, default value */
            SensorKombi,        /**< Kombisensor which has temperature, humidity, dew point,
                                     wind and rain */
            SensorPool,         /**< Pool sensor which only has temperature */
            SensorNormal        /**< sensor with temperature and humidity */
        };

    public:
        /**
         * @brief Constructor
         *
         * Creates a new UsbWde1Dataset object with default data.
         */
        UsbWde1Dataset();

        /**
         * @brief Virtual destructor.
         *
         * This destructor does nothing but exists for inheritance reasons.
         */
        virtual ~UsbWde1Dataset() {}

    public:

        /**
         * @brief Returns the sensor type
         *
         * The sensor type determines which values are valid.
         *
         * @return the sensor type
         */
        SensorType sensorType() const;

        /**
         * @brief Sets the sensor type
         *
         * @param[in] type the sensor type
         */
        void setSensorType(SensorType type);

        /**
         * @brief Returns the timestamp of the measurement
         *
         * @return the timestamp
         */
        bw::Datetime timestamp() const;

        /**
         * @brief Sets the timestamp of the measurement
         *
         * @param[in] timestamp the new timestamp
         */
        void setTimestamp(const bw::Datetime &timestamp);

        /**
         * @brief Returns the temperature
         *
         * @return the temperature in 1/100 degrees Celsius (°C).
         */
        int temperature() const;

        /**
         * @brief Sets the temperature of the data set
         *
         * @param[in] temperature the new temperature in 1/100 degrees Celsius (°C).
         */
        void setTemperature(int temperature);

        /**
         * @brief Returns the humidity
         *
         * @return the humidity in percent 1/100 (%)
         */
        int humidity() const;

        /**
         * @brief Sets the humidity
         *
         * @param[in] humidity the humidity in 1/100 percent (%)
         */
        void setHumidity(int humidity);

        /**
         * @brief Returns the wind speed
         *
         * @return the wind speed in 1/100 km/h.
         */
        int windSpeed() const;

        /**
         * @brief Set the wind speed
         *
         * @param[in] windSpeed the wind speed in 1/100 km/h.
         */
        void setWindSpeed(int windSpeed);

        /**
         * @brief Returns the rain gauge
         *
         * @return the rain gauge in ticks.
         */
        int rainGauge() const;

        /**
         * @brief Sets the rain gauge
         *
         * @param[in] rainGauge the rain gauge in ticks.
         */
        void setRainGauge(int rainGauge);

        /**
         * @brief Returns the rain status.
         *
         * @return @c true if it's raining, @c false if not.
         */
        bool isRain() const;

        /**
         * @brief Sets the rain status
         *
         * @param[in] rain @c true if it rains, @c false otherwise.
         */
        void setIsRain(bool rain);

        /**
         * @brief Creates a string-represenation of the object
         *
         * This saves us a friend declaration and also makes it easier to use C-like debugging
         * frameworks.
         *
         * @return the string representation for debugging
         */
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

/**
 * @brief Prints a UsbWde1Dataset object
 *
 * @param[in] os the output stream
 * @param[in] dataset the data set object
 * @return the output stream
 */
std::ostream &operator<<(std::ostream &os, const UsbWde1Dataset &dataset);

/* }}} */
/* CurrentWeather {{{ */

/**
 * @class CurrentWeather
 * @brief Current weather data
 *
 * This is a container class for the current weather. The difference to the UsbWde1Dataset is that
 * this class contains the accumulated rain amount of a day and the min/max/avg values of the day.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */
class CurrentWeather
{
    public:
        /**
         * @brief Constructor
         *
         * Creates a new CurrentWeather object.
         */
        CurrentWeather();

        /**
         * @brief Virtual destructor.
         *
         * This destructor does nothing but exists for inheritance reasons.
         */
        virtual ~CurrentWeather() {}

    public:
        /**
         * @brief Returns the timestamp of the measurement
         *
         * @return the timestamp
         */
        bw::Datetime timestamp() const;

        /** @name Temperature */                                                           /* {{{ */
        /** @{ */

        /**
         * @brief Sets the timestamp of the measurement
         *
         * @param[in] timestamp the new timestamp
         */
        void setTimestamp(const bw::Datetime &timestamp);

        /**
         * @brief Returns the current temperature
         *
         * @return the current temperature in 1/100 °C.
         */
        int temperature() const;

        /**
         * @brief Returns the current temperature as floating-point
         *
         * @return the current temperature in °C.
         */
        double temperatureReal() const;

        /**
         * @brief Sets the current temperature
         *
         * @param[in] temperature the current temperature in 1/100 °C.
         */
        void setTemperature(int temperature);

        /**
         * @brief Returns the minimum temperature of the day
         *
         * @return the minimum temperature in 1/100 °C.
         */
        int minTemperature() const;

        /**
         * @brief Returns the minimum temperature of the day as floating-point
         *
         * @return the current temperature in °C.
         */
        double minTemperatureReal() const;

        /**
         * @brief Sets the minimum temperature of the day
         *
         * @param[in] minTemperature the minimum temperature in 1/100 °C.
         */
        void setMinTemperature(int minTemperature);

        /**
         * @brief Returns the maximum temperature of the day
         *
         * @return the maximum temperature in 1/100 °C.
         */
        int maxTemperature() const;

        /**
         * @brief Returns the maximum temperature of the day
         *
         * @return the maximum temperature in °C.
         */
        double maxTemperatureReal() const;

        /**
         * @brief Sets the maximum temperature of the day
         *
         * @param[in] maxTemperature the maximum temperature in 1/00 °C.
         */
        void setMaxTemperature(int maxTemperature);

        /** @} */                                                                          /* }}} */

        /** @name Humidity and Dewpoint */                                                 /* {{{ */
        /** @{ */

        /**
         * @brief Returns the current humidity
         *
         * @return the current humidity in 1/100 %
         */
        int humidity() const;

        /**
         * @brief Returns the current humidity as floating-point
         *
         * @return the current humidity in %
         */
        double humidityReal() const;

        /**
         * @brief Sets the current humidity
         *
         * @param[in] humidity the humidity in 1/100 %
         */
        void setHumidity(int humidity);

        /**
         * @brief Returns the current dew point
         *
         * @return the current dew point in 1/100 °C.
         */
        int dewpoint() const;

        /**
         * @brief Returns the current dew point as floating-point
         *
         * @return the current dew point in °C.
         */
        double dewpointReal() const;

        /**
         * @brief Sets the current dew point
         *
         * @param[in] dewpoint the dew point in 1/100 °C.
         */
        void setDewpoint(int dewpoint);

        /** @} */                                                                          /* }}} */

        /** @name Wind */                                                                  /* {{{ */
        /** @{ */

        /**
         * @brief Returns the current wind speed in 1/100 km/h
         *
         * @return the current wind speed in 1/100 km/h
         */
        int windSpeed() const;

        /**
         * @brief Returns the current wind speed as floating-point
         *
         * @return the current wind speed in km/h
         */
        double windSpeedReal() const;

        /**
         * @brief Sets the current wind speed in 1/100 km/h
         *
         * @param[in] windSpeed the current wind speed in 1/100 km/h
         */
        void setWindSpeed(int windSpeed);

        /**
         * @brief Returns the wind speed in Beaufort
         *
         * @return a number between 0 and 12.
         */
        int windBeaufort() const;

        /**
         * @brief Sets the wind strength in Beaufort
         *
         * @param[in] bft a number between 0 and 12
         */
        void setWindBeaufort(int bft);

        /**
         * @brief Returns the maximum wind speed
         *
         * @return the wind maximum speed in km/h.
         */
        int maxWindSpeed() const;

        /**
         * @brief Returns the maximum wind speed as floating-point
         *
         * @return the wind maximum speed in km/h.
         */
        double maxWindSpeedReal() const;

        /**
         * @brief Set the maximum wind speed
         *
         * @param[in] windSpeed the maximum wind speed in km/h.
         */
        void setMaxWindSpeed(int windSpeed);

        /**
         * @brief Returns the maximum wind speed
         *
         * @return the wind maximum speed in Beaufort.
         */
        int maxWindBeaufort() const;

        /**
         * @brief Returns the maximum wind speed
         *
         * @param[in] bft the wind maximum speed in Beaufort.
         */
        void setMaxWindBeaufort(int bft);

        /** @} */                                                                          /* }}} */

        /** @name Rain */                                                                  /* {{{ */
        /** @{ */

        /**
         * @brief Returns the accumulated rain amount
         *
         * @return the rain amount of the day in 1/1000 l/m²
         */
        int rain() const;

        /**
         * @brief Returns the accumulated rain amount as floating point
         *
         * @return the rain amount of the day in l/m²
         */
        double rainReal() const;

        /**
         * @brief Sets the accumulated rain amount
         *
         * @param[in] rain the rain amount of the day in 1/1000 l/m²
         */
        void setRain(int  rain);

        /** @} */                                                                          /* }}} */

        /**
         * @brief Creates a string-represenation of the object
         *
         * This saves us a friend declaration and also makes it easier to use C-like debugging
         * frameworks.
         *
         * @return the string representation for debugging
         */
        std::string str() const;

    private:
        bw::Datetime m_timestamp;
        int m_temperature, m_minTemperature, m_maxTemperature;
        int m_humidity, m_dewpoint;
        int m_windSpeed, m_maxWindSpeed, m_windBft, m_maxWindBft;
        int m_rain;
};

/**
 * @brief Prints a CurrentWeather object
 *
 * @param[in] os the output stream
 * @param[in] dataset the data set object
 * @return the output stream
 */
std::ostream &operator<<(std::ostream &os, const CurrentWeather &dataset);

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // DATASET_H
