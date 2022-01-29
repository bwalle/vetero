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
#ifndef VETERO_COMMON_WEATHER_H_
#define VETERO_COMMON_WEATHER_H_

#include <stdexcept>
#include <string>
#include <cstdarg>

namespace vetero::common {

namespace weather {

/**
 * \brief Converts a wind speed to a wind force in Beaufort
 *
 * \param[in] kmh the wind speed in kmh
 * \return a value between 0 and 12
 */
int windSpeedToBft(double kmh);

/**
 * \brief Integer version of windSpeedToBft()
 *
 * \param[in] kmh the wind speed in 1/100 km/h
 * \return a value between 0 and 12
 */
int windSpeedToBft(int kmh);

/**
 * \brief Calculates the dewpoint
 *
 * \param[in] temp the temperature in degrees Celsius
 * \param[in] humid the humidity in %
 * \return the dewpoint in degrees Celsius
 */
double dewpoint(double temp, double humid);

/**
 * \brief Integer version of dewpoint()
 *
 * \param[in] temp the temperature in 1/100 degrees Celsius
 * \param[in] humid the humidity in 1/100 %
 * \return the dewpoint in 1/100 degrees Celsius
 */
int dewpoint(int temp, int humid);

/**
 * \brief Converts degrees Celsius to degrees Fahrenheit
 *
 * \param[in] celsius the temperature in degrees Celsius
 * \return the temperature in degrees Fahrenheit
 */
double celsius2Fahrenheit(double celsius);

/**
 * \brief Converts km/h (wind speed) to mph
 *
 * \param[in] kmh wind speed in km/h
 * \return wind speed in mph
 */
double kmh2mph(double kmh);

/**
 * \brief Converts rain mm to inch
 *
 * \param[in] mm rain in mm
 * \return rain value in in
 */
double mm2in(double mm);

/**
 * \brief Converts presure from hPa to inHg
 *
 * \param[in] hPa pressure in hPa
 * @return pressure in inHg
 */
double hPa2inHg(double hPa);

}

} // end namespace vetero::common

#endif // VETERO_COMMON_WEATHER_H_
