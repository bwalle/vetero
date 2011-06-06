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

#include "database.h"

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
         * @brief Inserts a new UsbWde1Dataset object
         *
         * @param[in] dataset the dataset to insert
         * @exception DatabaseError if the dataset could not be inserted
         */
        void insertUsbWde1Dataset(const UsbWde1Dataset &dataset) const
        throw (DatabaseError);

        /**
         * @brief Returns the last inserted weather data value
         *
         * @return the weather data object or an invalid object if the database is empty
         * @exception DatabaseError if the dataset could not be queried
         */
        UsbWde1Dataset queryLastInserted() const
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
         * @brief Adds or update an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @param[in] value the value as string
         * @param[in] insert @c true if a new item should be inserted (<tt>INSERT</tt>) or
         *            @c false if an existing item should be updated (<tt>UPDATE</tt>)
         * @exception DatabaseError if executing of the SQL statement failed
         */
        void writeMiscEntry(const std::string &key, const std::string &value, bool insert) const
        throw (DatabaseError);

        /**
         * @brief Reads an entry in the misc key/value table
         *
         * @param[in] key the name of the key
         * @return the string or an empty string if @p key doesn't exist.
         * @exception DatabaseError if executing of the SQL statement failed
         */
        std::string readMiscEntry(const std::string &key) const
        throw (DatabaseError);

    protected:
        /**
         * @brief Queries UsbWde1Dataset data
         *
         * @param[in] whereClause the @c WHERE clause of the SQL, for example
         *            <tt>DATE(timestamp) = '2011-01-01'</tt>. Can be empty to return
         *            the whole data set (not recommended).
         * @return the data converted to UsbWde1Dataset values
         */
        std::vector<UsbWde1Dataset> queryData(const std::string &whereClause) const
        throw (DatabaseError);

    private:
        Database *m_db;
};

/* }}} */

} // end namespace common
} // end namespace vetero

#endif // DBACCESS_H
