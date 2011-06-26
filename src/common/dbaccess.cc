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
#include <libbw/stringutil.h>
#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>

#include "common/error.h"
#include "dbaccess.h"
#include "weather.h"

namespace vetero {
namespace common {

/* DbAccess {{{ */

// -------------------------------------------------------------------------------------------------
const char *DbAccess::LastRain                  = "last_rain";
const char *DbAccess::DatabaseSchemaRevision    = "db_revision";

// -------------------------------------------------------------------------------------------------
DbAccess::DbAccess(Database *db)
    : m_db(db)
{}

// -------------------------------------------------------------------------------------------------
void DbAccess::initTables() const
    throw (DatabaseError)
{
    // TABLE misc
    m_db->executeSql(
        "CREATE TABLE misc ("
        "    key          STRING UNIQUE PRIMARY KEY,"
        "    value        STRING"
        ")"
    );

    // TABLE weatherdata
    m_db->executeSql(
        "CREATE TABLE weatherdata ("
        "    timestamp    DATETIME PRIMARY KEY UNIQUE,"
        "    temp         INTEGER,"
        "    humid        INTEGER,"
        "    dewpoint     INTEGER,"
        "    wind         INTEGER,"
        "    wind_bft     INTEGER,"
        "    rain         INTEGER"
        ")"
    );

    // TABLE day_statistics
    m_db->executeSql(
        "CREATE TABLE day_statistics ("
        "    date         DATE PRIMARY KEY UNIQUE,"
        "    temp_min     INTEGER,"
        "    temp_max     INTEGER,"
        "    temp_avg     INTEGER,"
        "    humid_min    INTEGER,"
        "    humid_max    INTEGER,"
        "    humid_avg    INTEGER,"
        "    dewpoint_min INTEGER,"
        "    dewpoint_max INTEGER,"
        "    dewpoint_avg INTEGER,"
        "    wind_min     INTEGER,"
        "    wind_max     INTEGER,"
        "    wind_avg     INTEGER,"
        "    wind_bft_min INTEGER,"
        "    wind_bft_max INTEGER,"
        "    wind_bft_avg INTEGER,"
        "    rain         INTEGER"
        ")"
    );

    // TABLE month_statistics
    m_db->executeSql(
        "CREATE TABLE month_statistics ("
        "    month        TEXT PRIMARY KEY UNIQUE,"
        "    temp_min     INTEGER,"
        "    temp_max     INTEGER,"
        "    temp_avg     INTEGER,"
        "    humid_min    INTEGER,"
        "    humid_max    INTEGER,"
        "    humid_avg    INTEGER,"
        "    dewpoint_min INTEGER,"
        "    dewpoint_max INTEGER,"
        "    dewpoint_avg INTEGER,"
        "    wind_min     INTEGER,"
        "    wind_max     INTEGER,"
        "    wind_avg     INTEGER,"
        "    wind_bft_min INTEGER,"
        "    wind_bft_max INTEGER,"
        "    wind_bft_avg INTEGER,"
        "    rain         INTEGER"
        ")"
    );

    //
    // convencience views with floating point
    //

    // VIEW weatherdata_float
    m_db->executeSql(
        "CREATE VIEW weatherdata_float AS SELECT"
        "    timestamp                        AS timestamp,"
        "    round(temp/100.0, 1)             AS temp,"
        "    round(humid/100.0, 0)            AS humid,"
        "    round(dewpoint/100.0, 1)         AS dewpoint,"
        "    round(wind/100.0, 1)             AS wind,"
        "    wind_bft                         AS wind_bft,"
        "    round(rain/1000.0, 1)            AS rain "
        "FROM weatherdata"
    );

    // VIEW day_statistics_float
    m_db->executeSql(
        "CREATE VIEW day_statistics_float AS SELECT"
        "    date                             AS date,"
        "    round(temp_min/100.0, 1)         AS temp_min,"
        "    round(temp_max/100.0, 1)         AS temp_max,"
        "    round(temp_avg/100.0, 1)         AS temp_avg,"
        "    round(humid_min/100.0, 0)        AS humid_min,"
        "    round(humid_max/100.0, 0)        AS humid_max,"
        "    round(humid_avg/100.0, 0)        AS humid_avg,"
        "    round(dewpoint_min/100.0, 1)     AS dewpoint_min,"
        "    round(dewpoint_max/100.0, 1)     AS dewpoint_max,"
        "    round(dewpoint_avg/100.0, 1)     AS dewpoint_avg,"
        "    round(wind_min/100.0, 1)         AS wind_min,"
        "    round(wind_max/100.0, 1)         AS wind_max,"
        "    round(wind_avg/100.0, 1)         AS wind_avg,"
        "    round(wind_bft_min/1000.0, 1)    AS wind_bft_min,"
        "    round(wind_bft_max/1000.0, 1)    AS wind_bft_max,"
        "    round(wind_bft_avg/1000.0, 1)    AS wind_bft_avg,"
        "    round(rain/1000.0, 1)            AS rain "
        "FROM day_statistics"
    );

    // VIEW month_statistics_float
    m_db->executeSql(
        "CREATE VIEW month_statistics_float AS SELECT"
        "    month                            AS month,"
        "    round(temp_min/100.0, 1)         AS temp_min,"
        "    round(temp_max/100.0, 1)         AS temp_max,"
        "    round(temp_avg/100.0, 1)         AS temp_avg,"
        "    round(humid_min/100.0, 0)        AS humid_min,"
        "    round(humid_max/100.0, 0)        AS humid_max,"
        "    round(humid_avg/100.0, 0)        AS humid_avg,"
        "    round(dewpoint_min/100.0, 1)     AS dewpoint_min,"
        "    round(dewpoint_max/100.0, 1)     AS dewpoint_max,"
        "    round(dewpoint_avg/100.0, 1)     AS dewpoint_avg,"
        "    round(wind_min/100.0, 1)         AS wind_min,"
        "    round(wind_max/100.0, 1)         AS wind_max,"
        "    round(wind_avg/100.0, 1)         AS wind_avg,"
        "    round(wind_bft_min/1000.0, 1)    AS wind_bft_min,"
        "    round(wind_bft_max/1000.0, 1)    AS wind_bft_max,"
        "    round(wind_bft_avg/1000.0, 1)    AS wind_bft_avg,"
        "    round(rain/1000.0, 1)            AS rain "
        "FROM month_statistics"
    );

    writeMiscEntry(DatabaseSchemaRevision, 1);
}

// -------------------------------------------------------------------------------------------------
void DbAccess::writeMiscEntry(const std::string &key, const std::string &value) const
    throw (DatabaseError)
{
    m_db->executeSql("INSERT OR REPLACE INTO misc (key, value) VALUES (?, ?)",
                     key.c_str(), value.c_str());
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
void DbAccess::insertUsbWde1Dataset(const UsbWde1Dataset &dataset) const
    throw (DatabaseError)
{
    // rain calculation
    int rain = 0;
    if (dataset.sensorType() == UsbWde1Dataset::SensorKombi) {
        int lastRain = readMiscEntry(LastRain, -1);
        if (lastRain == -1)
            lastRain = dataset.rainGauge();
        int rainGaugeDiff = dataset.rainGauge() - lastRain;
        if (rainGaugeDiff < 0)
            rainGaugeDiff += 4096 + 1;
        rain = rainGaugeDiff * UsbWde1Dataset::RAIN_GAUGE_FACTOR;
    }

    // wind bft
    int windStrength = 0;
    if (dataset.sensorType() == UsbWde1Dataset::SensorKombi)
        windStrength = Weather::windSpeedToBft(dataset.windSpeed());

    // dew point calculation
    int dewpoint = 0;
    if (dataset.sensorType() == UsbWde1Dataset::SensorKombi ||
            dataset.sensorType() == UsbWde1Dataset::SensorNormal)
        dewpoint = Weather::dewpoint(dataset.temperature(), dataset.humidity());

    m_db->executeSql("INSERT INTO weatherdata "
                     "(timestamp, temp, humid, dewpoint, wind, wind_bft, rain) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?)",
                     dataset.timestamp().str().c_str(),
                     bw::str( dataset.temperature() ).c_str(),
                     bw::str( dataset.humidity() ).c_str(),
                     bw::str( dewpoint ).c_str(),
                     bw::str( dataset.windSpeed() ).c_str(),
                     bw::str( windStrength ).c_str(),
                     bw::str( rain ).c_str() );

    if (dataset.sensorType() == UsbWde1Dataset::SensorKombi)
        writeMiscEntry(LastRain, dataset.rainGauge());
}

// -------------------------------------------------------------------------------------------------
CurrentWeather DbAccess::queryCurrentWeather() const
    throw (DatabaseError)
{
    CurrentWeather ret;

    Database::DbResultVector result = m_db->executeSqlQuery(
        "SELECT   strftime('%%s', timestamp), temp, humid, dewpoint, wind, wind_bft, rain "
        "FROM     weatherdata "
        "ORDER BY timestamp DESC "
        "LIMIT 1"
    );
    if (result.size() == 0 || result.at(0).size() == 0)
        return ret;

    std::vector<std::string> data = result.front();
    ret.setTimestamp( bw::Datetime(bw::from_str<time_t>(data.at(0))) );
    ret.setTemperature( bw::from_str<int>(data.at(1)) );
    ret.setHumidity( bw::from_str<int>(data.at(2)) );
    ret.setDewpoint( bw::from_str<int>(data.at(3)) );
    ret.setWindSpeed( bw::from_str<int>(data.at(4)) );
    ret.setWindBeaufort( bw::from_str<int>(data.at(5)) );

    result = m_db->executeSqlQuery(
        "SELECT   temp_min, temp_max, wind_max, wind_bft_max, rain "
        "FROM     day_statistics "
        "WHERE    date = ?", ret.timestamp().strftime("%Y-%m-%d").c_str()
    );

    if (result.size() == 0 || result.at(0).size() == 0) {
        ret.setMinTemperature(ret.temperature());
        ret.setMaxTemperature(ret.temperature());
        ret.setMaxWindSpeed(ret.windSpeed());
        ret.setMaxWindBeaufort(ret.windBeaufort());
    } else {
        data = result.front();
        ret.setMinTemperature( bw::from_str<int>(data.at(0)) );
        ret.setMaxTemperature( bw::from_str<int>(data.at(1)) );
        ret.setMaxWindSpeed( bw::from_str<int>(data.at(2)) );
        ret.setMaxWindBeaufort( bw::from_str<int>(data.at(3)) );
        ret.setRain( bw::from_str<int>(data.at(4)) );
    }

    return ret;
}

// -------------------------------------------------------------------------------------------------
std::vector<std::string> DbAccess::dataDays(bool nocache) const
    throw (DatabaseError)
{
    std::vector<std::string> ret;

    common::Database::DbResultVector result;
    if (nocache)
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT STRFTIME('%%Y-%%m-%%d', timestamp) AS d "
            "FROM       weatherdata "
            "ORDER BY   d"
        );
    else
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT date "
            "FROM       day_statistics "
            "ORDER BY   date"
        );

    common::Database::DbResultVector::const_iterator it;
    for (it = result.begin(); it != result.end(); ++it)
        ret.push_back(it->front());

    return ret;
}

// -------------------------------------------------------------------------------------------------
std::vector<std::string> DbAccess::dataMonths(bool nocache) const
    throw (DatabaseError)
{
    std::vector<std::string> ret;

    common::Database::DbResultVector result;
    if (nocache)
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT STRFTIME('%%Y-%%m', timestamp) AS m "
            "FROM       weatherdata "
            "ORDER BY   m"
        );
    else
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT month "
            "FROM       month_statistics "
            "ORDER BY   month"
        );

    common::Database::DbResultVector::const_iterator it;
    for (it = result.begin(); it != result.end(); ++it)
        ret.push_back(it->front());

    return ret;
}

// -------------------------------------------------------------------------------------------------
std::vector<std::string> DbAccess::dataYears(bool nocache) const
    throw (DatabaseError)
{
    std::vector<std::string> ret;

    common::Database::DbResultVector result;
    if (nocache)
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT STRFTIME('%%Y', timestamp) AS m "
            "FROM       weatherdata "
            "ORDER BY   m"
        );
    else
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT SUBSTR(month, 0, 5) "
            "FROM       month_statistics "
            "ORDER BY   month"
        );

    common::Database::DbResultVector::const_iterator it;
    for (it = result.begin(); it != result.end(); ++it)
        ret.push_back(it->front());

    return ret;
}

// -------------------------------------------------------------------------------------------------
void DbAccess::updateDayStatistics(const std::string &date)
    throw (DatabaseError)
{
    if (date.empty()) {
        std::vector<std::string> days = dataDays(true);

        std::vector<std::string>::const_iterator it;;
        for (it = days.begin(); it != days.end(); ++it)
            updateDayStatistics(*it);

        return;
    }

    BW_DEBUG_INFO("Regenerating day statistics for %s", date.c_str());

    m_db->executeSql(
        "INSERT OR REPLACE INTO day_statistics "
        "(date, temp_min, temp_max, temp_avg, "
        " humid_min, humid_max, humid_avg, "
        " dewpoint_min, dewpoint_max, dewpoint_avg, "
        " wind_min, wind_max, wind_avg, "
        " wind_bft_min, wind_bft_max, wind_bft_avg, "
        " rain) "
        " SELECT  ?, MIN(temp), MAX(temp), ROUND(AVG(temp)), "
        "         MIN(humid), MAX(humid), ROUND(AVG(humid)), "
        "         MIN(dewpoint), MAX(dewpoint), ROUND(AVG(dewpoint)), "
        "         MIN(wind), MAX(wind), ROUND(AVG(wind)), "
        "         VETERO_BEAUFORT(MIN(wind)), VETERO_BEAUFORT(MAX(wind)), VETERO_BEAUFORT(AVG(wind)), "
        "         SUM(rain) "
        "  FROM   weatherdata "
        "  WHERE  DATE(timestamp) = ?",
        date.c_str(), date.c_str()
    );
}

// -------------------------------------------------------------------------------------------------
void DbAccess::updateMonthStatistics(const std::string &month)
    throw (DatabaseError)
{
    if (month.empty()) {
        std::vector<std::string> months = dataMonths(true);

        std::vector<std::string>::const_iterator it;;
        for (it = months.begin(); it != months.end(); ++it)
            updateMonthStatistics(*it);

        return;
    }

    BW_DEBUG_INFO("Regenerating month statistics for %s", month.c_str());

    m_db->executeSql(
        "INSERT OR REPLACE INTO month_statistics "
        "(month, temp_min, temp_max, temp_avg, "
        " humid_min, humid_max, humid_avg, "
        " dewpoint_min, dewpoint_max, dewpoint_avg, "
        " wind_min, wind_max, wind_avg, "
        " wind_bft_min, wind_bft_max, wind_bft_avg, "
        " rain) "
        " SELECT  ?, MIN(temp_min), MAX(temp_max), ROUND(AVG(temp_avg)), "
        "         MIN(humid_min), MAX(humid_max), ROUND(AVG(humid_avg)), "
        "         MIN(dewpoint_min), MAX(dewpoint_max), ROUND(AVG(dewpoint_avg)), "
        "         MIN(wind_min), MAX(wind_max), ROUND(AVG(wind_avg)), "
        "         VETERO_BEAUFORT(MIN(wind_min)), VETERO_BEAUFORT(MAX(wind_max)), "
        "         VETERO_BEAUFORT(AVG(wind_avg)), "
        "         SUM(rain) "
        "  FROM   day_statistics "
        "  WHERE  STRFTIME('%%Y-%%m', date) = ?",
        month.c_str(), month.c_str()
    );
}

/* }}} */

} // end namespace common
} // end namespace vetero
