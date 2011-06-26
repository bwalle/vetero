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
#ifndef DBACCESS_H
#define DBACCESS_H

#include <vector>

#include <libbw/stringutil.h>

#include "database.h"
#include "common/error.h"

/**
 * @file
 * @brief Read and write values from/to the DB
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */

namespace vetero {
namespace common {

/* DbAccess {{{ */

/**
 * @class DbAccess
 * @brief Read and write values to/from the database
 *
 * @author Bernhard Walle <bernhard@bwalle.de>
 * @ingroup common
 */
class DbAccess {

    public:
        /// Constant to query the last rain gauge in ticks with readMiscEntry()
        static const char *LastRain;

        /// Constant to query or set the database schema revision
        static const char *DatabaseSchemaRevision;

        /**
         * @brief Constructor
         *
         * @param[in] db the connection to the database. The object is not owned by the
         *            DbAccess and must be valid during the whole lifetime of DbAccess.
         */
        DbAccess(Database *db);

    public:
        /**
         * @brief Initializes the tables
         *
         * Calling this function is only valid if the tables don't exist. This function creates
         * the @c weather_data and @c misc tables.
         *
         * @exception if the tables cannot be creates
         */
        void initTables() const
        throw (DatabaseError);

        /**
         * @brief Registers temporary views used in the application.
         *
         * This function must be called before any other function of this class
         * is called, but after initTables().
         *
         * @exception DatabaseError if the tables cannot be created
         */
        void initViews() const
        throw (DatabaseError);

        /**
         * @brief Add or update an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @param[in] value the value as string
         * @exception DatabaseError if executing of the SQL statement failed
         */
        void writeMiscEntry(const std::string &key, const std::string &value) const
        throw (DatabaseError);

        /**
         * @brief Add or update an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @param[in] value the value as string
         * @exception DatabaseError if executing of the SQL statement failed
         */
        template <typename T>
        void writeMiscEntry(const std::string &key, const T &value) const
        throw (DatabaseError);

        /**
         * @brief Reads an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @return the string or an empty string if @p key doesn't exist.
         * @exception ApplicationError if executing of the SQL statement failed
         */
        std::string readMiscEntry(const std::string &key) const
        throw (DatabaseError);

        /**
         * @brief Reads an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @param[in] defaultValue the default value which is returned if the entry doesn't exist
         * @return the string or an empty string if @p key doesn't exist.
         * @exception ApplicationError if executing of the SQL statement failed
         */
        template <typename T>
        T readMiscEntry(const std::string &key, const T &defaultValue=T()) const
        throw (DatabaseError);

        /**
         * @brief Inserts a new UsbWde1Dataset object
         *
         * @param[in] dataset the dataset to insert
         * @exception DatabaseError if the dataset could not be inserted
         */
        void insertUsbWde1Dataset(const UsbWde1Dataset &dataset) const
        throw (DatabaseError);

        /**
         * @brief Queries the current weather from the database
         *
         * @return the current weather with all elements filled
         * @exception DatabaseError if the dataset could not be queried
         */
        CurrentWeather queryCurrentWeather() const
        throw (DatabaseError);

        /**
         * @brief Returns a vector with all days that have weather data
         *
         * @param[in] nocache if @c true, don't use the cached values from the database but check
         *            the raw data. This is mainly for internal use to generate the cache and is
         *            slow.
         * @return a (sorted) list of dates in the format <tt>YYYY-MM-DD</tt>
         */
        std::vector<std::string> dataDays(bool nocache=false) const
        throw (DatabaseError);

        /**
         * @brief Returns a vector with all months that have weather data
         *
         * @param[in] nocache if @c true, don't use the cached values from the database but check
         *            the raw data. This is mainly for internal use to generate the cache and is
         *            slow.
         * @return a (sorted) list of dates in the format <tt>YYYY-MM</tt>
         */
        std::vector<std::string> dataMonths(bool nocache=false) const
        throw (DatabaseError);

        /**
         * @brief Returns a vector with all years that have weather data
         *
         * @param[in] nocache if @c true, don't use the cached values from the database but check
         *            the raw data. This is mainly for internal use to generate the cache and is
         *            slow.
         * @return a (sorted) list of dates in the format <tt>YYYY-MM</tt>
         */
        std::vector<std::string> dataYears(bool nocache=false) const
        throw (DatabaseError);

        /**
         * @brief Updates the day statistics in the database
         *
         * This function should be called by veterod after a data set has been inserted. The reason
         * why it's not called automatically by a trigger is the fact that it is not possible to
         * call a trigger every day to update the month statistics. For symmetry reasons, the day
         * statistics are also updated manually.
         *
         * @param[in] date the date in the format <tt>YYYY-mm-dd</tt>. If empty, the statistics
         *            for all days are updated.
         * @exception DatabaseError if accessing the database failed
         */
        void updateDayStatistics(const std::string &date)
        throw (DatabaseError);

        /**
         * @brief Updates the month statistics in the database
         *
         * @param[in] month the date in the format <tt>YYYY-mm</tt>. If empty, the statistics for
         *            all months are updated.
         * @exception DatabaseError if accessing the database failed
         */
        void updateMonthStatistics(const std::string &month)
        throw (DatabaseError);

    private:
        Database *m_db;
};

/* Template implementation {{{ */

// -------------------------------------------------------------------------------------------------
template <typename T>
void DbAccess::writeMiscEntry(const std::string &key, const T &value) const
    throw (DatabaseError)
{
    return writeMiscEntry(key, bw::str(value));
}

// -------------------------------------------------------------------------------------------------
template <typename T>
T DbAccess::readMiscEntry(const std::string &key, const T &defaultValue) const
    throw (DatabaseError)
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

#endif // DBACCESS_H
