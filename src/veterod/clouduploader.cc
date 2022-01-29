/* {{{
 * (c) 2021, Bernhard Walle <bernhard@bwalle.de>
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

#include <sstream>
#include <iomanip>

#include <libbw/log/debug.h>
#include <libbw/log/errorlog.h>

#include <common/httprequest.h>
#include "clouduploader.h"

namespace vetero::daemon {

/**
 * \brief Uploader for the WeatherUnderground API
 *
 * API documentation: https://support.weather.com/s/article/PWS-Upload-Protocol?language=en_US
 */
class WeatherUndergroundUploader : public CloudUploader
{
public:
    WeatherUndergroundUploader(const common::Configuration &config) {
        m_stationId = config.cloudStationId();
        m_stationKey = config.cloudStationPassword();
    }

public:
    void upload(const common::CurrentWeather &weather) override {
        std::ostringstream url;
        url << "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php?"
            << "ID=" << m_stationId << "&"
            << "PASSWORD=" << m_stationKey << "&"
            << "tempf=" << std::setprecision(2) << weather.temperatureRealF() << "&";

        if (weather.hasHumidity())
            url << "humidity=" << std::setprecision(2) << weather.humidityReal() << "&"
                << "dewptf=" << std::setprecision(2) << weather.dewpointRealF() << "&";

        if (weather.hasWindSpeed())
            url << "windspeedmph=" << std::setprecision(2) << weather.windSpeedRealMph() << "&";

        if (weather.hasWindDirection())
            url << "winddir=" << weather.windDirection() << "&";

        if (weather.hasRain())
            url << "dailyrainin=" << std::setprecision(5) << weather.rainRealIn() << "&";

        if (weather.hasPressure())
            url << "baromin=" << std::setprecision(3) << weather.pressureRealIn() << "&";

        url << "dateutc=now&"
            << "action=updateraw";

        BW_DEBUG_DBG("WU URL: %s", url.str().c_str());

        common::HttpRequest req(url.str());
        try {
            req.perform();
        } catch (const common::HttpError &err) {
            BW_ERROR_WARNING("Unable to update WU station ID %s: %s", m_stationId.c_str(), err.what());
        }
    }

private:
    std::string m_stationId;
    std::string m_stationKey;

};


CloudUploader *CloudUploader::create(const std::string &type, const common::Configuration &config)
{
    if (type == "weatherunderground")
        return new WeatherUndergroundUploader(config);
    else
        return nullptr;
}

} // vetero::daemon
