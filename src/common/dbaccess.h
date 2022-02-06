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
#ifndef VETERO_COMMON_DBACCESS_H_
#define VETERO_COMMON_DBACCESS_H_

#include <vector>

#include <libbw/stringutil.h>
#include <libbw/noncopyable.h>

#include "database.h"
#include "common/error.h"
#include "progressnotifier.h"

namespace vetero {
namespace common {

/* DbAccess {{{ */

//
// \class DbAccess
// \brief Read and write values to/from the database
class DbAccess : private bw::Noncopyable
{
    public:
        /// Constant to query the last rain gauge in ticks with readMiscEntry()
        static const char *LastRain;

        /// Constant to query or set the database schema revision
        static const char *DatabaseSchemaRevision;

        DbAccess(Database *db);

    public:
        Database &database();
        const Database &database() const;

        void initTables() const;
        void initViews() const;

        void writeMiscEntry(const std::string &key, const std::string &value) const;

        template <typename T>
        void writeMiscEntry(const std::string &key, const T &value) const;

        std::string readMiscEntry(const std::string &key) const;

        template <typename T>
        T readMiscEntry(const std::string &key, const T &defaultValue=T()) const;

        void insertDataset(const Dataset &dataset, int &rainValue) const;

        CurrentWeather queryCurrentWeather() const;

        std::vector<std::string> dataDays(bool nocache=false) const;
        std::vector<std::string> dataMonths(bool nocache=false) const;
        std::vector<std::string> dataYears(bool nocache=false) const;

        void deleteStatistics();

        void updateDayStatistics(const std::string &date);
        void updateDayStatistics();

        void updateMonthStatistics(const std::string &month);
        void updateMonthStatistics();

        // Allows to set a progress notifier. Used in updateDayStatistics() and updateMonthStatistics().
        // NULL means no notifier. Ownership is not transferred to the DbAccess object, so you have to
        // manually delete it.
        void setProgressNotifier(ProgressNotifier *progress);

    private:
        Database *m_db;
        ProgressNotifier *m_progressNotifier;
};

/* Template implementation {{{ */

template <typename T>
void DbAccess::writeMiscEntry(const std::string &key, const T &value) const
{
    return writeMiscEntry(key, bw::str(value));
}

template <typename T>
T DbAccess::readMiscEntry(const std::string &key, const T &defaultValue) const
{
    std::string result = readMiscEntry(key);
    if (result.empty())
        return defaultValue;
    else
        return bw::from_str<T>(result);
}

/* }}} */

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // VETERO_COMMON_DBACCESS_H_
