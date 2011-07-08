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
#ifndef VETERO_COMMON_DATABASE_H_
#define VETERO_COMMON_DATABASE_H_

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <libbw/noncopyable.h>

#include <sqlite3.h>

#include "error.h"
#include "dataset.h"

namespace vetero {
namespace common {

/* Database {{{ */

/**
 * \class Database
 * \brief Abstract class for database access
 *
 * This is a simple abstraction for the database access. The class needs to be re-implemented
 * by the concrete database.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class Database : private bw::Noncopyable {

    public:
        /**
         * \brief Result of a SQL query as returned by executeQuery()
         *
         * The first layer represents the lines and the second layer represents the column.
         * For example consider the table \c "Patients":
         *
         * <table>
         *   <tr>
         *      <td><b>Id</b></td>
         *      <td><b>Name</b></td>
         *      <td><b>Weight</b></td>
         *   </tr>
         *   <tr>
         *      <td>0</td>
         *      <td>John Doe</td>
         *      <td>80</td>
         *   </tr>
         *   <tr>
         *      <td>1</td>
         *      <td>Jane Doe</td>
         *      <td>60</td>
         *   </tr>
         * </table>
         *
         * Then the SQL query <tt>"SELECT * FROM Patients"</tt> would return
         *
         * \code
         * result = {
         *               {    "0",        "John Doe",     "80" },
         *               {    "1",        "Jane Doe",     "60" },
         *          }
         * \endcode
         */
        typedef std::vector< std::vector<std::string> > DbResultVector;

    public:
        /**
         * \brief Destructor.
         */
        virtual ~Database() {}

        /**
         * \brief Opens the connection to the database
         *
         * The function may throw exceptions on error. Which exceptions is described in the
         * documentation of the concrete implementation.
         *
         * \param[in] connection the connection string. What that means is documented in the documentation
         *            of the concrete implementation.
         * \param[in] flags flags for the connection. What that means is documented in the documentation
         *            of the concrete implementation.
         */
        virtual void open(const std::string &connection, int flags) = 0;

        /**
         * \brief Closes the connection to the database
         *
         * It's not valid to throw exceptions for the function.
         */
        virtual void close() = 0;

        /**
         * \brief Executes a SQL query that doesn't return results
         *
         * Use this function to create table or to manipulate the data in the tables.
         *
         * Using the placeholder character <tt>'?'</tt> (without quotes) makes sure that the values
         * will be quoted correctly so SQL injection bugs can be avoided. Each element in the
         * varardic argument list must be of type <tt>const char *</tt>.
         *
         * \code
         * const char *sql = "INSERT INTO table (a, b) VALUES (?, ?)";
         * db->executeSql(sql, "1", "2");
         * \endcode
         *
         * \warning Format strings like <tt>%s</tt> that should be passed to the database literally
         *          like for builtin functions need to be written with a double percentage like
         *          <tt>%%%s</tt>. Example: <tt>"strftime('%%s', ...)"</tt>.
         *
         * \note The default implementation calls vexecuteSqlQuery().
         *
         * \param[in] sql the SQL that should be processed. Each occurrence of <tt>'?'</tt> (without
         *            quotes) will be replaced by the counterpart in argument list.
         *            elements.
         * \exception DatabaseError if the SQL statement cannot be executed
         * \see executeSqlQuery()
         */
        virtual void executeSql(const char *sql, ...)
        throw (DatabaseError);

        /**
         * \brief Executes a SQL query and return the results
         *
         * Use this function to query data from the database. See also the description of the
         * ResultVector type for a description how the result can be used.
         *
         * Using the placeholder character <tt>'?'</tt> (without quotes) makes sure that the values
         * will be quoted correctly so SQL injection bugs can be avoided. Each element in the
         * varardic argument list must be of type <tt>const char *</tt>.
         *
         * \code
         * const char *sql = "INSERT INTO table (a, b) VALUES (?, ?)";
         * db->executeSql(sql, "1", "2");
         * \endcode
         *
         * \warning Format strings like <tt>%s</tt> that should be passed to the database literally
         *          like for builtin functions need to be written with a double percentage like
         *          <tt>%%%s</tt>. Example: <tt>"strftime('%%s', ...)"</tt>.
         *
         * \note The default implementation calls vexecuteSqlQuery().
         *
         * \param[in] sql the SQL that should be processed. Each occurrence of <tt>'?'</tt> (without
         *            quotes) will be replaced by the counterpart in argument list.
         *            elements.
         * \return the result vector as described in DbResultVector
         * \exception DatabaseError if the SQL statement cannot be executed
         */
        virtual DbResultVector executeSqlQuery(const char *sql, ...)
        throw (DatabaseError);

    protected:
        /**
         * \brief Varardic variant of vexecuteSqlQuery
         *
         * Instead of re-implementing executeSql() and executeSqlQuery() an implementation can just
         * reimplement this function.
         *
         * \note If the implementation decides to re-implement executeSql() and executeSqlQuery(),
         *       it's not needed to implement this function. However, an empty body must be provided
         *       to avoid linker errors.
         *
         * \param[in] sql the SQL string
         * \param[in] args the varardic args
         * \return the result vector as described in DbResultVector
         * \exception DatabaseError if the SQL statement cannot be executed
         */
        virtual DbResultVector vexecuteSqlQuery(const char *sql, va_list args)
        throw (DatabaseError) = 0;
};

/* }}} */
/* Sqlite3Database {{{ */

/**
 * \class Sqlite3Database
 * \brief Access to the SQLite3 on-disk database
 *
 * This class contains methods to read and write data from the SQLite3 database file.
 *
 * \author Bernhard Walle <bernhard@bwalle.de>
 * \ingroup common
 */
class Sqlite3Database : public Database {

    public:
        /**
         * \brief Constructor
         */
        Sqlite3Database();

        /**
         * \brief Destructor
         *
         * Calls close().
         */
        ~Sqlite3Database();

    public:
        /**
         * Flags for Sqlite3Database::open().
         */
        enum SqliteOpenFlags {
            FLAG_READONLY = (1<<0)      /**< open the database readonly */
        };

        /**
         * \brief Opens the database connection
         *
         * If the database doesn't exist, it will created and the table will be created.
         *
         * Creates a new Database accessor
         *
         * \param[in] connection the name to the SQLite3 database file. If it doesn't exist, it will
         *            be created.
         * \param[in] flags a bitwise-or combination of flags in SqliteOpenFlags.
         * \exception DatabaseError if the database cannot be created or creating the table failed
         * \exception std::bad_alloc if SQLite3 was not able to allocate memory for the connection
         *            handle.
         * \see close()
         */
        virtual void open(const std::string &connection, int flags)
        throw (DatabaseError, std::bad_alloc);

        /**
         * \copydoc Database::close()
         */
        virtual void close();

    protected:
        /**
         * \copydoc Database::vexecuteSqlQuery()
         */
        virtual DbResultVector vexecuteSqlQuery(const char *sql, va_list args)
        throw (DatabaseError);

        /**
         * \brief Registers custom database functions
         *
         * \exception DatabaseError if registering fails
         */
        void registerCustomFunctions()
        throw (DatabaseError);

    private:
        sqlite3     *m_connection;
};

/* }}} */

} // end namespace common
} // end namespace vetero

/* Print result vector {{{ */

/**
 * \brief Converts a DB result vector in something printable
 *
 * \param[in] os the output stream
 * \param[in] vector the database result vector
 * \return \p os
 */
std::ostream &operator<<(std::ostream &os, const vetero::common::Database::DbResultVector &vector);

/* }}} */

#endif // VETERO_COMMON_DATABASE_H_
