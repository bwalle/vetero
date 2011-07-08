/* {{{
 * (c) 2011, Bernhard Walle <bernhard@bwalle.de>
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
#ifndef WEATHER_H
#define WEATHER_H

#include <stdexcept>
#include <string>
#include <cstdarg>

namespace vetero {
namespace common {

/**
 * @brief Weather-related mathematical  functions
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */
class Weather
{
    public:
        /**
         * @brief Convets a wind speed to a wind force in Beaufort
         *
         * @param[in] kmh the wind speed in kmh
         * @return a value between 0 and 12
         */
        static int windSpeedToBft(double kmh);

        /**
         * @brief Integer version of windSpeedToBft()
         *
         * @param[in] kmh the wind speed in 1/100 km/h
         * @return a value between 0 and 12
         */
        static int windSpeedToBft(int kmh);

        /**
         * @brief Calculates the dewpoint
         *
         * @param[in] temp the temperature in degrees Celsius
         * @param[in] humid the humidity in %
         * @return the dewpoint in degrees Celsius
         */
        static double dewpoint(double temp, double humid);

        /**
         * @brief Integer version of dewpoint()
         *
         * @param[in] temp the temperature in 1/100 degrees Celsius
         * @param[in] humid the humidity in 1/100 %
         * @return the dewpiont in 1/100 degrees Celsius
         */
        static int dewpoint(int temp, int humid);
};

} // end namespace common
} // end namespace vetero

#endif // WEATHER_H
