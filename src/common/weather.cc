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

#include <cmath>

#include "weather.h"

namespace vetero::common {

namespace weather {

int windSpeedToBft(double windspeed)
{
    int kmh = static_cast<int>(round(windspeed));

    if (kmh < 1)
        return 0;
    else if (kmh <= 5)
        return 1;
    else if (kmh <= 11)
        return 2;
    else if (kmh <= 19)
        return 3;
    else if (kmh <= 28)
        return 4;
    else if (kmh <= 38)
        return 5;
    else if (kmh <= 49)
        return 6;
    else if (kmh <= 61)
        return 7;
    else if (kmh <= 74)
        return 8;
    else if (kmh <= 88)
        return 9;
    else if (kmh <= 102)
        return 10;
    else if (kmh <= 117)
        return 11;
    else
        return 12;
}

int windSpeedToBft(int windspeed)
{
    return windSpeedToBft(windspeed/100.0);
}

double dewpoint(double temp, double humid)
{
    return (241.2 * std::log(humid/100.0) + ((4222.03716*temp)/(241.2+temp))) /
            (17.5043 - log(humid/100.0) - ((17.5043*temp)/(241.2+temp)));
}

int dewpoint(int temp, int humid)
{
    double dp = dewpoint(temp/100.0, humid/100.0);
    return static_cast<int>(round(dp*100.0));
}

double celsius2Fahrenheit(double celsius)
{
    return celsius * 1.8 + 32;
}

double kmh2mph(double kmh)
{
    return kmh * 0.62137;
}

double mm2in(double mm)
{
    return mm / 25.4;
}

double hPa2inHg(double hPa)
{
    return hPa * 0.029529980164712;
}

double calculateSeaLevelPressure(double height, double pressure)
{
    if (height < 0.1)
        return pressure;

    return pressure / pow(1.0 - height/44330.0, 5.255);
}

} // end namespace weather

} // end namespace vetero::common
