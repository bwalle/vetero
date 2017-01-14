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
#ifndef VETERO_REPORTGEN_VETERO_REPORTGEN_H_
#define VETERO_REPORTGEN_VETERO_REPORTGEN_H_

#include <string>
#include <memory>

#include "common/error.h"
#include "common/configuration.h"
#include "common/database.h"
#include "common/veteroapplication.h"
#include "validdatacache.h"

namespace vetero {
namespace reportgen {

/**
 * \class VeteroReportgen
 * \brief Report generation class
 *
 * This is the main class for the report generation tool.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup report
 */
class VeteroReportgen : public common::VeteroApplication
{
    public:
        /**
         * \brief Constructor
         */
        VeteroReportgen();

    public:
        /**
         * \brief Returns a reference to the DB access object.
         *
         * \return the DB access object.
         */
        vetero::common::Sqlite3Database &database();

        /**
         * \brief Returns a reference to the configuration object
         *
         * \return the reference
         */
        vetero::common::Configuration &configuration();

        /**
         * \brief Returns the valid data cache object
         *
         * \return a reference to the object
         */
        const ValidDataCache &validDataCache() const;

        /**
         * \brief Returns a reference to the configuration object
         *
         * \return the const reference
         */
        const vetero::common::Configuration &configuration() const;

        /**
         * \brief Reads the configuration file
         *
         * If a configuration file has been specified on the command line, read the
         * configuration file and throw an exception if the configuration file cannot be
         * read. If no configuration file has been specified, check if the default configuration
         * file exists and if yes, read it. Only if the file exists and cannot be parsed, an
         * \c common::ApplicationError is thrown.
         *
         * \exception common::ApplicationError if the configuration file cannot be read or parsed,
         *            see above for more information.
         */
        void readConfiguration();

        /**
         * \brief Parse the command line
         *
         * \param[in] argc the number of arguments
         * \param[in] argv the arguments
         * \return \c true if the application should be continued,
         *         \c false if the application should be quit
         * \exception ApplicationError if parsing the command line failed.
         */
        bool parseCommandLine(int argc, char *argv[]);

        /**
         * \brief Opens the database connection
         *
         * Opens the database as specified on the command line. If it doesn't exist, the database
         * will be created.
         *
         * \exception common::ApplicationError if it's not possible to create the database.
         */
        void openDatabase();

        /**
         * \brief Main loop of the application
         *
         * This is the main part of the application.
         */
        void exec();

    protected:
        /**
         * \brief Performs the upload of reports
         */
        void uploadReports();

    private:
        common::Sqlite3Database m_database;
        std::unique_ptr<common::DbAccess> m_dbAccess;
        std::unique_ptr<ValidDataCache> m_validDataCache;
        std::vector<std::string> m_jobs;

        std::string m_configfile;
        bool m_noConfigFatal;
        std::unique_ptr<vetero::common::Configuration> m_configuration;

        bool m_upload;
};

} // end namespace reportgen
} // end namespace vetero

#endif // VETERO_REPORTGEN_VETERO_REPORTGEN_H_
