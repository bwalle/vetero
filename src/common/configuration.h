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

        //  Pressure sensor

        int pressureSensorI2cBus() const;
        int pressureHeight() const;

        // Database

        std::string databasePath() const;

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

        std::string str() const;

    protected:
        void read(const std::string &filename);

    private:
        std::string m_serialDevice;
        int         m_serialBaud;
        SensorType  m_sensorType;
        int         m_pressureSensorI2cBus;
        int         m_pressureHeight;
        std::string m_reportTitleColor1;
        std::string m_reportTitleColor2;
        std::string m_reportDirectory;
        std::string m_reportUploadCommand;
        std::string m_locationString;
        std::string m_databasePath;
        std::string m_displayName;
        std::string m_displayConnection;
        bool        m_configurationRead;
        std::string m_locale;
        std::string m_error;
};

} // end namespace common
} // end namespace vetero

std::ostream &operator<<(std::ostream &os, vetero::common::Configuration &config);

#endif // VETERO_COMMON_CONFIGURATION_H_
