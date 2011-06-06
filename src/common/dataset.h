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

/**
 * @file
 * @brief Contains a container classes.
 *
 * This files contains container classes for weather data.
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */

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
         * @return the temperature in degrees Celsius (°C).
         */
        double temperature() const;

        /**
         * @brief Sets the temperature of the data set
         *
         * @param[in] temperature the new temperature in degrees Celsius (°C).
         */
        void setTemperature(double temperature);

        /**
         * @brief Returns the humidity
         *
         * @return the humidity in percent (%)
         */
        int humidity() const;

        /**
         * @brief Sets the humidity
         *
         * @param[in] humidity the humidity in percent (%)
         */
        void setHumidity(int humidity);

        /**
         * @brief Returns the current dew point
         *
         * @return the current dew point in °C.
         */
        double dewpoint() const;

        /**
         * @brief Sets the current dew point
         *
         * @param[in] dewpoint the dew point in °C.
         */
        void setDewpoint(double dewpoint);

        /**
         * @brief Returns the wind speed
         *
         * @return the wind speed in km/h.
         */
        double windSpeed() const;

        /**
         * @brief Set the wind speed
         *
         * @param[in] windSpeed the wind speed in km/h.
         */
        void setWindSpeed(double windSpeed);

        /**
         * @brief Returns the wind speed in Beaufort.
         *
         * @return the wind speed in Beaufort between 0 and 12.
         */
        int windSpeedBeaufort() const;

        /**
         * @brief Returns the rain gauge
         *
         * @return the rain gauge in ticks.
         */
        unsigned long long rainGauge() const;

        /**
         * @brief Sets the rain gauge
         *
         * @param[in] rainGauge the rain gauge in ticks.
         */
        void setRainGauge(unsigned long long rainGauge);

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
        bw::Datetime m_timestamp;
        double m_temperature;
        int m_humidity;
        double m_dewpoint;
        double m_windSpeed;
        unsigned long long m_rainGauge;
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

        /**
         * @brief Sets the timestamp of the measurement
         *
         * @param[in] timestamp the new timestamp
         */
        void setTimestamp(const bw::Datetime &timestamp);

        /**
         * @brief Returns the current temperature
         *
         * @return the current temperature in °C.
         */
        double temperature() const;

        /**
         * @brief Sets the current temperature
         *
         * @param[in] temperature the current temperature in °C.
         */
        void setTemperature(double temperature);

        /**
         * @brief Returns the minimum temperature of the day
         *
         * @return the minimum temperature in °C.
         */
        double minTemperature() const;

        /**
         * @brief Sets the minimum temperature of the day
         *
         * @param[in] minTemperature the minimum temperature in °C.
         */
        void setMinTemperature(double minTemperature);

        /**
         * @brief Returns the maximum temperature of the day
         *
         * @return the maximum temperature in °C.
         */
        double maxTemperature() const;

        /**
         * @brief Sets the maximum temperature of the day
         *
         * @param[in] maxTemperature the maximum temperature in °C.
         */
        void setMaxTemperature(double maxTemperature);

        /**
         * @brief Returns the current humidity
         *
         * @return the current humidity in %
         */
        int humidity() const;

        /**
         * @brief Sets the current humidity
         *
         * @param[in] humidity the humidity in %
         */
        void setHumidity(int humidity);

        /**
         * @brief Returns the current dew point
         *
         * @return the current dew point in °C.
         */
        double dewpoint() const;

        /**
         * @brief Sets the current dew point
         *
         * @param[in] dewpoint the dew point in °C.
         */
        void setDewpoint(double dewpoint);

        /**
         * @brief Returns the current wind speed in km/h
         *
         * @return the current wind speed in km/h
         */
        double windSpeed() const;

        /**
         * @brief Sets the current wind speed in km/h
         *
         * @param[in] windSpeed the current wind speed in km/h
         */
        void setWindSpeed(double windSpeed);

        /**
         * @brief Returns the wind speed in Beaufort
         *
         * @return a number between 0 and 12.
         */
        int windBeaufort() const;

        /**
         * @brief Returns the maximum wind speed
         *
         * @return the wind maximum speed in km/h.
         */
        double maxWindSpeed() const;

        /**
         * @brief Set the maximum wind speed
         *
         * @param[in] windSpeed the maximum wind speed in km/h.
         */
        void setMaxWindSpeed(double windSpeed);

        /**
         * @brief Returns the maximum wind speed
         *
         * @return the wind maximum speed in Beaufort.
         */
        int maxWindBeaufort() const;

        /**
         * @brief Returns the accumulated rain amount
         *
         * @return the rain amount of the day in mm (= l/m²)
         */
        double rain() const;

        /**
         * @brief Sets the accumulated rain amount
         *
         * @param[in] rain the rain amount of the day in mm (= l/m²)
         */
        void setRain(double rain);

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
        bw::Datetime m_timestamp;
        double m_temperature, m_minTemperature, m_maxTemperature;
        int m_humidity;
        double m_dewpoint;
        double m_windSpeed, m_maxWindSpeed;
        double m_rain;
        bool m_IsRain;
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
