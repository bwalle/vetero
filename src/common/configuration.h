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
#ifndef VETERO_COMMON_CONFIGURATION_H_
#define VETERO_COMMON_CONFIGURATION_H_

#include <cstdio>

#include "common/error.h"
#include "common/dataset.h"

namespace vetero {
namespace common {

class Configuration
{
    public:
        Configuration(const std::string &filename="");
        virtual ~Configuration() {}

    public:
        bool configurationRead() const;
        std::string error() const;

        // Serial device access

        std::string serialDevice() const;
        int serialBaud() const;

        // Sensor configuration

        SensorType sensorType() const;
        int sensorNumber() const;
        int pressureHeight() const;

        // Database

        std::string databasePath() const;
        std::string updatePostscript() const;

        // Report generation

        std::string reportTitleColor1() const;
        std::string reportTitleColor2() const;
        std::string reportDirectory() const;
        std::string reportUploadCommand() const;
        std::string locationString() const;
        std::string locale() const;

        // LCD

        std::string displayName() const;
        std::string displayConnection() const;

        // cloud

        std::string cloudType() const;
        std::string cloudStationId() const;
        std::string cloudStationPassword() const;

        std::string str() const;

    protected:
        void read(const std::string &filename);

    private:
        std::string m_serialDevice = "/dev/ttyS0";
        int         m_serialBaud = 9600;
        SensorType  m_sensorType = SensorType::Kombi;
        int         m_sensorNumber = -1;
        int         m_pressureHeight = -1;
        std::string m_reportTitleColor1 = "#217808";
        std::string m_reportTitleColor2 = "#91d007";
        std::string m_reportDirectory;
        std::string m_reportUploadCommand;
        std::string m_locationString;
        std::string m_databasePath = "vetero.db";
        std::string m_updatePostscript;
        std::string m_displayName;
        std::string m_displayConnection;
        bool        m_configurationRead = false;
        std::string m_locale;
        std::string m_error;
        std::string m_cloudType;
        std::string m_cloudStationId;
        std::string m_cloudStationPassword;
};

} // end namespace common
} // end namespace vetero

std::ostream &operator<<(std::ostream &os, vetero::common::Configuration &config);

#endif // VETERO_COMMON_CONFIGURATION_H_
