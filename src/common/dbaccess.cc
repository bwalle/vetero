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
#include <cstdlib>

#include <libbw/stringutil.h>
#include <libbw/log/errorlog.h>

#include "dbaccess.h"

namespace vetero {
namespace common {

/* DbAccess {{{ */

// -------------------------------------------------------------------------------------------------
DbAccess::DbAccess(Database *db)
    : m_db(db)
{}

// -------------------------------------------------------------------------------------------------
void DbAccess::initTables() const
    throw (DatabaseError)
{
    m_db->executeSql("CREATE TABLE weatherdata ("
                     "    id          INTEGER PRIMARY KEY,"
                     "    timestamp   DATETIME,"
                     "    temp        REAL,"
                     "    humid       REAL,"
                     "    rain_gauge  INTEGER,"
                     "    is_rain     BOOL,"
                     "    wind        REAL "
                     ")");
    m_db->executeSql("CREATE TABLE misc ("
                     "    id          INTEGER PRIMARY KEY,"
                     "    key         STRING UNIQUE,"
                     "    value       STRING"
                     ")");
    m_db->executeSql("CREATE TABLE rain_min ("
                     "    date        DATE PRIMARY KEY UNIQUE,"
                     "    min_rain    INTEGER"
                     ")");
    m_db->executeSql("CREATE TRIGGER update_rain_min AFTER INSERT ON weatherdata "
                     "BEGIN "
                     "  INSERT OR REPLACE INTO rain_min (date, min_rain) "
                     "  VALUES ( date(new.timestamp), "
                     "           ( SELECT min(rain_gauge) FROM weatherdata "
                     "             WHERE  date(timestamp) = date(new.timestamp) )"
                     "         ); "
                     "END;");

    writeMiscEntry("rain_gauge_offset", "0", true);
}

// -------------------------------------------------------------------------------------------------
void DbAccess::initViews() const
    throw (DatabaseError)
{
    m_db->executeSql("CREATE TEMP VIEW weatherdata_extended AS  "
                     "SELECT    id, "
                     "          timestamp, "
                     "          temp, "
                     "          humid, "
                     "          VETERO_DEWPOINT(temp, humid) AS dewpoint, "
                     "          rain_gauge, "
                     "          0.295 * ( rain_gauge - (SELECT min_rain "
                     "                        FROM rain_min "
                     "                        WHERE date(weatherdata.timestamp) = rain_min.date) ) AS rain, "
                     "          is_rain, "
                     "          wind, "
                     "          VETERO_BEAUFORT(wind) AS wind_beaufort "
                     "FROM      weatherdata");

    m_db->executeSql("CREATE TEMP VIEW day_statistics AS "
                     "SELECT    DATE(timestamp)             AS date, "
                     "          MIN(temp)                   AS temp_min, "
                     "          MAX(temp)                   AS temp_max, "
                     "          AVG(temp)                   AS temp_avg, "
                     "          MIN(humid)                  AS min_humid, "
                     "          MAX(humid)                  AS max_humid, "
                     "          MIN(wind)                   AS wind_min, "
                     "          MAX(wind)                   AS wind_max, "
                     "          AVG(wind)                   AS wind_avg, "
                     "          VETERO_BEAUFORT(MAX(wind))  AS wind_max_beaufort, "
                     "          VETERO_BEAUFORT(AVG(wind))  AS wind_avg_beaufort, "
                     "          0.295 * ( MAX(rain_gauge) - (SELECT min_rain "
                     "                        FROM rain_min "
                     "                        WHERE date(weatherdata_extended.timestamp) = rain_min.date) ) "
                     "                                      AS rain, "
                     "          0.295 * ( MAX(rain_gauge) - (SELECT min_rain "
                     "                        FROM rain_min "
                     "                        WHERE strftime('%%Y-%%m-01', weatherdata_extended.timestamp) = "
                     "                              rain_min.date) ) AS rain_month "
                     "FROM      weatherdata_extended "
                     "GROUP BY  date(timestamp) "
                     "HAVING    temp_min <> temp_max "
                     "ORDER BY  date");
}

// -------------------------------------------------------------------------------------------------
void DbAccess::insertUsbWde1Dataset(const UsbWde1Dataset &dataset) const
    throw (DatabaseError)
{
    m_db->executeSql("INSERT INTO weatherdata "
                     "(timestamp, temp, humid, rain_gauge, is_rain, wind) "
                     "VALUES (?, ?, ?, ?, ?, ?)",
                     dataset.timestamp().str().c_str(),
                     bw::str( dataset.temperature() ).c_str(),
                     bw::str( dataset.humidity() ).c_str(),
                     bw::str( dataset.rainGauge() ).c_str(),
                     bw::str( dataset.isRain() ).c_str(),
                     bw::str( dataset.windSpeed() ).c_str() );
}

// -------------------------------------------------------------------------------------------------
UsbWde1Dataset DbAccess::queryLastInserted() const
    throw (DatabaseError)
{
    std::vector<UsbWde1Dataset> data = queryData("id = (SELECT MAX(id) FROM weatherdata)");
    if (data.empty())
        return UsbWde1Dataset();
    else
        return data[0];
}

// -------------------------------------------------------------------------------------------------
CurrentWeather DbAccess::queryCurrentWeather() const
    throw (DatabaseError)
{
    CurrentWeather current;
    UsbWde1Dataset lastInserted = queryLastInserted();

    current.setTimestamp(lastInserted.timestamp());
    current.setTemperature(lastInserted.temperature());
    current.setHumidity(lastInserted.humidity());
    current.setDewpoint(lastInserted.dewpoint());
    current.setWindSpeed(lastInserted.windSpeed());
    current.setIsRain(lastInserted.isRain());

    // defaults

    //
    // min/max values
    //

    Database::DbResultVector result = m_db->executeSqlQuery(
        "SELECT temp_min, temp_max, wind_max, rain "
        "FROM   day_statistics "
        "WHERE  date = date('now', 'localtime')");

    if (!result.empty() && result.at(0).size() == 4) {
        current.setMinTemperature( std::atof(result.at(0).at(0).c_str()) );
        current.setMaxTemperature( std::atof(result.at(0).at(1).c_str()) );
        current.setMaxWindSpeed( std::atof(result.at(0).at(2).c_str()) );
        current.setRain( std::atof(result.at(0).at(3).c_str()) );
    } else {
        BW_ERROR_WARNING("Unable to retrieve day statistics for current values");
        current.setMinTemperature(current.temperature());
        current.setMaxTemperature(current.temperature());
        current.setMaxWindSpeed(current.windSpeed());
        current.setRain(0.0);
    }

    return current;
}

// -------------------------------------------------------------------------------------------------
void DbAccess::writeMiscEntry(const std::string &key, const std::string &value, bool insert) const
    throw (DatabaseError)
{
    std::string sql;
    if (insert)
        sql = "INSERT INTO misc (key, value) VALUES (?, ?)";
    else
        sql = "UPDATE misc SET value = ? WHERE key = ?";

    m_db->executeSql(sql.c_str(), value.c_str(), key.c_str());
}

// -------------------------------------------------------------------------------------------------
std::string DbAccess::readMiscEntry(const std::string &key) const
    throw (DatabaseError)
{
    std::string sql = "SELECT value FROM misc WHERE key = ?";
    Database::DbResultVector result = m_db->executeSqlQuery(sql.c_str(), key.c_str());

    if (result.empty())
        return std::string();
    else {
        if (result.size() != 1 || result[0].size() != 1)
            throw DatabaseError("Invalid result returned. SQL was '"+ sql +"'.");
        return result[0][0];
    }
}

// -------------------------------------------------------------------------------------------------
std::vector<UsbWde1Dataset> DbAccess::queryData(const std::string &whereClause) const
    throw (DatabaseError)
{
    std::string sql = "SELECT strftime('%%s', datetime(timestamp, 'utc')), "
                      "       temp, humid, dewpoint, rain_gauge, is_rain, wind "
                      "FROM weatherdata_extended ";
    if (!whereClause.empty())
        sql += "WHERE " + whereClause + " ";
    sql += "ORDER BY timestamp";

    Database::DbResultVector result = m_db->executeSqlQuery(sql.c_str());

    std::vector<UsbWde1Dataset> ret;
    for (Database::DbResultVector::const_iterator it = result.begin(); it != result.end(); ++it) {
        const std::vector<std::string> &row = *it;

        if (row.size() != 7)
            throw DatabaseError("Requested columns of size 7, got size " + bw::str(row.size()) );

        UsbWde1Dataset newData;
        newData.setTimestamp( bw::Datetime( std::strtol(row[0].c_str(), NULL, 10) ) );
        newData.setTemperature( std::atof(row[1].c_str()) );
        newData.setHumidity( std::atoi(row[2].c_str()) );
        newData.setDewpoint( std::atof(row[3].c_str()) );
        newData.setRainGauge( std::atoi(row[4].c_str()) );
        newData.setIsRain( row[5] == "true" );
        newData.setWindSpeed( std::atof(row[6].c_str()) );

        ret.push_back(newData);
    }

    return ret;
}

/* }}} */

} // end namespace common
} // end namespace vetero
