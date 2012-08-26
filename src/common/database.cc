/* {{{
 * (c) 2011-2012, Bernhard Walle <bernhard@bwalle.de>
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
#include <stdexcept>
#include <sstream>
#include <cassert>
#include <cmath>

#include <libbw/stringutil.h>

#include <sqlite3.h>

#include "database.h"
#include "utils.h"
#include "weather.h"

namespace vetero {
namespace common {

/* Database {{{ */

void Database::executeSql(const char *sql, ...)
{
    va_list ap;

    va_start(ap, sql);
    vexecuteSqlQuery(sql, ap);
    va_end(ap);
}

Database::DbResultVector Database::executeSqlQuery(const char *sql, ...)
{
    va_list ap;

    va_start(ap, sql);
    Database::DbResultVector ret = vexecuteSqlQuery(sql, ap);
    va_end(ap);

    return ret;
}

/* }}} */
/* function for the database {{{ */

static void sqlite3_beaufort(sqlite3_context* ctx, int number, sqlite3_value **values)
{
    assert(number == 1);
    assert(values != NULL);
    assert(values[0] != NULL);

    sqlite3_result_int(ctx, Weather::windSpeedToBft(sqlite3_value_int(values[0])));
}

/* }}} */
/* Sqlite3Database {{{ */

Sqlite3Database::Sqlite3Database()
    : m_connection(NULL)
{}

Sqlite3Database::~Sqlite3Database()
{
    Sqlite3Database::close();
}

void Sqlite3Database::open(const std::string &connection, int flags)
{
    int sqlite3_flags = 0;

    if (flags & FLAG_READONLY)
        sqlite3_flags |= SQLITE_OPEN_READONLY;
    else
        sqlite3_flags |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

    int err = sqlite3_open_v2(connection.c_str(), &m_connection, sqlite3_flags, NULL);
    if (err != SQLITE_OK) {
        if (m_connection)
            throw DatabaseError("Can't open database: " + std::string(sqlite3_errmsg(m_connection)) );
        else
            throw std::bad_alloc();
    }

    err = sqlite3_busy_timeout(m_connection, 20000);
    if (err != SQLITE_OK)
        throw DatabaseError("Unable to call sqlite3_busy_timeout(): " +
                            std::string(sqlite3_errmsg(m_connection)) );

    registerCustomFunctions();
}

void Sqlite3Database::close()
{
    if (m_connection) {
        sqlite3_close(m_connection);
        m_connection = NULL;
    }
}

static int vetero_sqlite3_callback(void *cookie, int columns, char **values, char **columnNames)
{
    Database::DbResultVector *results = static_cast<Database::DbResultVector *>(cookie);

    std::vector< std::string > line;

    for (int i = 0; i < columns; i++)
        line.push_back( values[i] ? std::string(values[i]) : std::string() );

    results->push_back(line);

    return SQLITE_OK;
}

Database::DbResultVector Sqlite3Database::vexecuteSqlQuery(const char *sql, va_list ap)
{
    char *finished_sql = sqlite3_vmprintf(bw::replace_char(sql, '?', "%Q").c_str(), ap);
    if (!finished_sql)
        throw DatabaseError("Unable to call sqlite3_vmprintf('"+ std::string(sql) +"')");

    char *errorstring;
    DbResultVector results;
    int ret = sqlite3_exec(m_connection,
                           finished_sql,
                           vetero_sqlite3_callback,
                           &results,
                           &errorstring);
    if (ret != SQLITE_OK) {
        std::stringstream ss;
        ss << "Unable to execute SQL ";
        ss << "(" << finished_sql << ")";
        ss << ": " << errorstring;
        sqlite3_free(errorstring);
        sqlite3_free(finished_sql);
        throw DatabaseError(ss.str());
    }

    sqlite3_free(errorstring);
    sqlite3_free(finished_sql);

    return results;
}

void Sqlite3Database::registerCustomFunctions()
{
    // register 'VETERO_BEAUFORT' function
    int err = sqlite3_create_function(
        m_connection,         // handle
        "VETERO_BEAUFORT",    // function name
        1,                    // number of arguments
        SQLITE_UTF8,          // preferred encoding
        NULL,                 // cookie pointer
        sqlite3_beaufort,     // xFunc
        NULL,                 // xStep (aggregate only)
        NULL                  // xFinal (aggregate only)
    );

    if (err != SQLITE_OK)
        throw DatabaseError("Unable to register 'BEAUFORT' function: " +
                            std::string(sqlite3_errmsg(m_connection)) );
}

/* }}} */

} // end namespace common
} // end namespace vetero

/* Print result vector {{{ */

std::ostream &operator<<(std::ostream &os, const vetero::common::Database::DbResultVector &vector)
{
    for (int i = 0; i < vector.size(); i++) {
        for (int j = 0; j < vector[i].size(); j++) {
            os << vector[i][j];
            if (j + 1 != vector[i].size())
                os << "|";
        }
        if (i + 1 != vector.size())
            os << std::endl;
    }

    return os;
}

/* }}} */
