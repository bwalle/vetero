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

#ifndef VETERO_CLOUDUPLOADER_H_
#define VETERO_CLOUDUPLOADER_H_

#include <common/dataset.h>
#include <common/configuration.h>

namespace vetero::daemon {

/**
 * \brief Interface for a cloud uploader
 *
 * This class cannot be instantiated, use CloudUploader::create().
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 */
class CloudUploader
{
public:
    /**
     * \brief Destroy the Cloud Uploader object
     */
    virtual ~CloudUploader() {}

public:
    /**
     * \brief Uploads the current weather data to the cloud
     *
     * @param weather
     */
    virtual void upload(const common::CurrentWeather &weather) = 0;

public:
    static CloudUploader *create(const std::string &type, const common::Configuration &config);

};

} // vetero::daemon

#endif // VETERO_CLOUDUPLOADER_H_