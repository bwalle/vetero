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
#include <libbw/stringutil.h>
#include <libbw/log/errorlog.h>
#include <libbw/log/debug.h>

#include "common/error.h"
#include "dbaccess.h"
#include "weather.h"

namespace vetero {
namespace common {

/* DummyProgressNotifier {{{ */

class DummyProgressNotifier : public ProgressNotifier {

public:
    virtual void progressed(double total, double now) {}
    virtual void finished() {}
};

namespace {
    DummyProgressNotifier dummyProgressNotifier;
}

/* }}} */
/* DbAccess {{{ */

const char *DbAccess::LastRain                  = "last_rain";
const char *DbAccess::DatabaseSchemaRevision    = "db_revision";

DbAccess::DbAccess(Database *db)
    : m_db(db),
      m_progressNotifier(&dummyProgressNotifier)
{}

Database &DbAccess::database()
{
    return *m_db;
}

const Database &DbAccess::database() const
{
    return *m_db;
}

void DbAccess::initTables() const
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
        "    timestamp       DATETIME PRIMARY KEY UNIQUE,"
        "    jdate           INTEGER,"
        "    temp            INTEGER,"
        "    humid           INTEGER,"
        "    dewpoint        INTEGER,"
        "    wind            INTEGER,"
        "    wind_bft        INTEGER,"
        "    wind_gust       INTEGER,"
        "    wind_gust_bft   INTEGER,"
        "    wind_dir        INTEGER,"
        "    solar_radiation INTEGER,"
        "    uv_index        INTEGER,"
        "    rain            INTEGER,"
        "    pressure        INTEGER"
        ")"
    );

    // TABLE day_statistics
    m_db->executeSql(
        "CREATE TABLE day_statistics ("
        "    date                 DATE PRIMARY KEY UNIQUE,"
        "    temp_min             INTEGER,"
        "    temp_max             INTEGER,"
        "    temp_avg             INTEGER,"
        "    humid_min            INTEGER,"
        "    humid_max            INTEGER,"
        "    humid_avg            INTEGER,"
        "    dewpoint_min         INTEGER,"
        "    dewpoint_max         INTEGER,"
        "    dewpoint_avg         INTEGER,"
        "    wind_min             INTEGER,"
        "    wind_max             INTEGER,"
        "    wind_avg             INTEGER,"
        "    wind_bft_min         INTEGER,"
        "    wind_bft_max         INTEGER,"
        "    wind_bft_avg         INTEGER,"
        "    wind_gust_min        INTEGER,"
        "    wind_gust_max        INTEGER,"
        "    wind_gust_avg        INTEGER,"
        "    wind_gust_bft_min    INTEGER,"
        "    wind_gust_bft_max    INTEGER,"
        "    wind_gust_bft_avg    INTEGER,"
        "    rain                 INTEGER"
        ")"
    );

    // TABLE month_statistics
    m_db->executeSql(
        "CREATE TABLE month_statistics ("
        "    month             TEXT PRIMARY KEY UNIQUE,"
        "    temp_min          INTEGER,"
        "    temp_max          INTEGER,"
        "    temp_avg          INTEGER,"
        "    humid_min         INTEGER,"
        "    humid_max         INTEGER,"
        "    humid_avg         INTEGER,"
        "    dewpoint_min      INTEGER,"
        "    dewpoint_max      INTEGER,"
        "    dewpoint_avg      INTEGER,"
        "    wind_min          INTEGER,"
        "    wind_max          INTEGER,"
        "    wind_avg          INTEGER,"
        "    wind_bft_min      INTEGER,"
        "    wind_bft_max      INTEGER,"
        "    wind_bft_avg      INTEGER,"
        "    wind_gust_min     INTEGER,"
        "    wind_gust_max     INTEGER,"
        "    wind_gust_avg     INTEGER,"
        "    wind_gust_bft_min INTEGER,"
        "    wind_gust_bft_max INTEGER,"
        "    wind_gust_bft_avg INTEGER,"
        "    rain              INTEGER"
        ")"
    );

    // INDEX index_weatherdata_jdate
    m_db->executeSql(
        "CREATE INDEX index_weatherdata_jdate "
        "ON weatherdata(jdate)"
    );

    // TRIGGER update_weatherdata_jday
    m_db->executeSql(
        "CREATE TRIGGER update_weatherdata_jday "
        "AFTER INSERT ON weatherdata "
        "BEGIN "
        "   UPDATE weatherdata "
        "   SET    jdate = julianday(strftime('%%Y-%%m-%%d 12:00', timestamp)) "
        "   WHERE  timestamp = new.timestamp; "
        "END"
    );

    //
    // convencience views with floating point
    //

    // VIEW weatherdata_float
    m_db->executeSql(
        "CREATE VIEW weatherdata_float AS SELECT"
        "    timestamp                        AS timestamp,"
        "    jdate                            AS jdate,"
        "    round(temp/100.0, 1)             AS temp,"
        "    round(humid/100.0, 0)            AS humid,"
        "    round(dewpoint/100.0, 1)         AS dewpoint,"
        "    round(wind/100.0, 1)             AS wind,"
        "    wind_bft                         AS wind_bft,"
        "    round(wind_gust/100.0, 1)        AS wind_gust,"
        "    wind_gust_bft                    AS wind_gust_bft,"
        "    wind_dir                         AS wind_dir,"
        "    round(solar_radiation/10.0, 1)   AS solar_radiation,"
        "    uv_index                         AS uv_index,"
        "    round(rain/1000.0, 3)            AS rain, "
        "    round(pressure/100.0, 0)         AS pressure "
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
        "    wind_bft_min                     AS wind_bft_min,"
        "    wind_bft_max                     AS wind_bft_max,"
        "    wind_bft_avg                     AS wind_bft_avg,"
        "    round(wind_gust_min/100.0, 1)    AS wind_gust_min,"
        "    round(wind_gust_max/100.0, 1)    AS wind_gust_max,"
        "    round(wind_gust_avg/100.0, 1)    AS wind_gust_avg,"
        "    wind_gust_bft_min                AS wind_gust_bft_min,"
        "    wind_gust_bft_max                AS wind_gust_bft_max,"
        "    wind_gust_bft_avg                AS wind_gust_bft_avg,"
        "    round(rain/1000.0, 1)            AS rain "
        "FROM day_statistics"
    );

    // VIEW month_statistics_float
    m_db->executeSql(
        "CREATE VIEW month_statistics_float AS SELECT"
        "    month                                AS month,"
        "    round(temp_min/100.0, 1)             AS temp_min,"
        "    round(temp_max/100.0, 1)             AS temp_max,"
        "    round(temp_avg/100.0, 1)             AS temp_avg,"
        "    round(humid_min/100.0, 0)            AS humid_min,"
        "    round(humid_max/100.0, 0)            AS humid_max,"
        "    round(humid_avg/100.0, 0)            AS humid_avg,"
        "    round(dewpoint_min/100.0, 1)         AS dewpoint_min,"
        "    round(dewpoint_max/100.0, 1)         AS dewpoint_max,"
        "    round(dewpoint_avg/100.0, 1)         AS dewpoint_avg,"
        "    round(wind_min/100.0, 1)             AS wind_min,"
        "    round(wind_max/100.0, 1)             AS wind_max,"
        "    round(wind_avg/100.0, 1)             AS wind_avg,"
        "    round(wind_bft_min/1000.0, 1)        AS wind_bft_min,"
        "    round(wind_bft_max/1000.0, 1)        AS wind_bft_max,"
        "    round(wind_bft_avg/1000.0, 1)        AS wind_bft_avg,"
        "    round(wind_gust_min/100.0, 1)        AS wind_gust_min,"
        "    round(wind_gust_max/100.0, 1)        AS wind_gust_max,"
        "    round(wind_gust_avg/100.0, 1)        AS wind_gust_avg,"
        "    round(wind_gust_bft_min/1000.0, 1)   AS wind_gust_bft_min,"
        "    round(wind_gust_bft_max/1000.0, 1)   AS wind_gust_bft_max,"
        "    round(wind_gust_bft_avg/1000.0, 1)   AS wind_gust_bft_avg,"
        "    round(rain/1000.0, 1)            AS rain "
        "FROM month_statistics"
    );

    writeMiscEntry(DatabaseSchemaRevision, 8);
}

void DbAccess::writeMiscEntry(const std::string &key, const std::string &value) const
{
    m_db->executeSql("INSERT OR REPLACE INTO misc (key, value) VALUES (?, ?)",
                     key.c_str(), value.c_str());
}

std::string DbAccess::readMiscEntry(const std::string &key) const
{
    std::string sql = "SELECT value FROM misc WHERE key = ?";
    Database::Result result = m_db->executeSqlQuery(sql.c_str(), key.c_str());

    if (result.data.empty())
        return std::string();
    else {
        if (result.data.size() != 1 || result.data[0].size() != 1)
            throw DatabaseError("Invalid result returned. SQL was '"+ sql +"'.");
        return result.data[0][0];
    }
}

void DbAccess::insertDataset(const Dataset &dataset, int &rainValue) const
{
    // rain calculation
    std::string rain("NULL");
    if (dataset.sensorType().hasRain()) {
        int lastRain = readMiscEntry(LastRain, -1);
        if (lastRain == -1)
            lastRain = dataset.rainGauge();
        int rainGaugeDiff = dataset.rainGauge() - lastRain;
        if (rainGaugeDiff < 0)
            rainGaugeDiff += 4096 + 1;
        rainValue = rainGaugeDiff * dataset.rainGaugeFactor();
        rain = bw::str(rainValue);
    } else {
        rainValue = -1;
    }

    // wind
    std::string windSpeed("NULL");
    std::string windStrength("NULL");
    if (dataset.sensorType().hasWindSpeed()) {
        windSpeed = bw::str( dataset.windSpeed() );
        windStrength = bw::str(weather::windSpeedToBft(dataset.windSpeed()));
    }

    // wind gust
    std::string windGust("NULL");
    std::string windGustStrength("NULL");
    if (dataset.sensorType().hasWindGust()) {
        windGust = bw::str( dataset.windGust() );
        windGustStrength = bw::str(weather::windSpeedToBft(dataset.windGust()));
    }

    // wind dir
    std::string windDirection("NULL");
    if (dataset.sensorType().hasWindDirection())
        windDirection = bw::str( dataset.windDirection() );

    // dew point calculation
    std::string humidity("NULL");
    std::string dewpoint("NULL");
    if (dataset.sensorType().hasHumidity()) {
        humidity = bw::str(dataset.humidity());
        dewpoint = bw::str(weather::dewpoint(dataset.temperature(), dataset.humidity()));
    }

    // solar radiation
    std::string solarRadiation("NULL");
    std::string uvIndex("NULL");
    if (dataset.sensorType().hasSolarRadiation()) {
        solarRadiation = bw::str(dataset.solarRadiation());
        uvIndex = bw::str(dataset.uvIndex());
    }

    // pressure
    std::string pressure("NULL");
    if (dataset.sensorType().hasPressure())
        pressure = bw::str(dataset.pressure());

    m_db->executeSql("INSERT INTO weatherdata "
                     "(timestamp, temp, humid, dewpoint, wind, wind_bft, wind_gust, wind_gust_bft, wind_dir, "
                     " solar_radiation, uv_index, pressure, rain) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                     c_str_null(dataset.timestamp().str()),
                     c_str_null(bw::str( dataset.temperature() )),
                     c_str_null(humidity), c_str_null(dewpoint),
                     c_str_null(windSpeed), c_str_null(windStrength),
                     c_str_null(windGust), c_str_null(windGustStrength),
                     c_str_null(windDirection),
                     c_str_null(solarRadiation), c_str_null(uvIndex),
                     c_str_null(pressure), c_str_null(rain) );

    if (dataset.sensorType().hasRain())
        writeMiscEntry(LastRain, dataset.rainGauge());
}

CurrentWeather DbAccess::queryCurrentWeather() const
{
    CurrentWeather ret;

    Database::Result result = m_db->executeSqlQuery(
        "SELECT   strftime('%%s', datetime(timestamp, 'utc')), "
        "         temp, humid, dewpoint, wind, wind_bft, wind_gust, wind_gust_bft, wind_dir, "
        "         solar_radiation, uv_index, pressure "
        "FROM     weatherdata "
        "ORDER BY timestamp DESC "
        "LIMIT 1"
    );
    if (result.data.size() == 0 || result.data.at(0).size() == 0)
        return ret;

    std::vector<std::string> data = result.data.front();
    ret.setTimestamp( bw::Datetime(bw::from_str<time_t>(data.at(0))) );
    ret.setTemperature( bw::from_str<int>(data.at(1)) );

    if (!data.at(2).empty()) {
        ret.setHumidity( bw::from_str<int>(data.at(2)) );
        ret.setDewpoint( bw::from_str<int>(data.at(3)) );
    }

    if (!data.at(4).empty()) {
        ret.setWindSpeed( bw::from_str<int>(data.at(4)) );
        ret.setWindBeaufort( bw::from_str<int>(data.at(5)) );
    }

    if (!data.at(6).empty()) {
        ret.setWindGust( bw::from_str<int>(data.at(6)) );
        ret.setWindGustBeaufort( bw::from_str<int>(data.at(7)) );
    }

    if (!data.at(8).empty())
        ret.setWindDirection( bw::from_str<int>(data.at(8)) );

    if (!data.at(9).empty())
        ret.setSolarRadiation( bw::from_str<int>(data.at(9)) );

    if (!data.at(10).empty())
        ret.setUvIndex( bw::from_str<int>(data.at(10)) );

    if (!data.at(11).empty())
        ret.setPressure( bw::from_str<int>(data.at(11)) );

    result = m_db->executeSqlQuery(
        "SELECT   temp_min, temp_max, wind_max, wind_gust_max, rain "
        "FROM     day_statistics "
        "WHERE    date = ?", ret.timestamp().strftime("%Y-%m-%d").c_str()
    );

    if (result.data.size() == 0 || result.data.at(0).size() == 0) {
        ret.setMinTemperature(ret.temperature());
        ret.setMaxTemperature(ret.temperature());
        ret.setMaxWindSpeed(ret.windSpeed());
        ret.setMaxWindGust(ret.windGust());
    } else {
        data = result.data.front();
        ret.setMinTemperature( bw::from_str<int>(data.at(0)) );
        ret.setMaxTemperature( bw::from_str<int>(data.at(1)) );
        ret.setMaxWindSpeed( bw::from_str<int>(data.at(2)) );
        ret.setMaxWindGust( bw::from_str<int>(data.at(3)) );
        if (!data.at(4).empty())
            ret.setRain( bw::from_str<int>(data.at(4)) );
    }

    return ret;
}

std::vector<std::string> DbAccess::dataDays(bool nocache) const
{
    std::vector<std::string> ret;

    common::Database::Result result;
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

    for (int i = 0; i < result.data.size(); ++i)
        ret.push_back(result.data[i].front());

    return ret;
}

std::vector<std::string> DbAccess::dataMonths(bool nocache) const
{
    std::vector<std::string> ret;

    common::Database::Result result;
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

    for (int i = 0; i < result.data.size(); ++i)
        ret.push_back(result.data[i].front());

    return ret;
}

std::vector<std::string> DbAccess::dataYears(bool nocache) const
{
    std::vector<std::string> ret;

    common::Database::Result result;
    if (nocache)
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT STRFTIME('%%Y', timestamp) AS y "
            "FROM       weatherdata "
            "ORDER BY   y ASC"
        );
    else
        result = m_db->executeSqlQuery(
            "SELECT     DISTINCT SUBSTR(month, 0, 5) "
            "FROM       month_statistics "
            "ORDER BY   month ASC"
        );

    for (int i = 0; i < result.data.size(); ++i)
        ret.push_back(result.data[i].front());

    return ret;
}

void DbAccess::deleteStatistics()
{
    m_db->executeSql("DELETE FROM day_statistics");
    m_db->executeSql("DELETE FROM month_statistics");
}

void DbAccess::updateDayStatistics(const std::string &date)
{
    if (date.empty())
        return updateDayStatistics();

    BW_DEBUG_INFO("Regenerating day statistics for %s", date.c_str());

    m_db->executeSql(
        "INSERT OR REPLACE INTO day_statistics "
        "(date, temp_min, temp_max, temp_avg, "
        " humid_min, humid_max, humid_avg, "
        " dewpoint_min, dewpoint_max, dewpoint_avg, "
        " wind_min, wind_max, wind_avg, "
        " wind_bft_min, wind_bft_max, wind_bft_avg, "
        " wind_gust_min, wind_gust_max, wind_gust_avg, "
        " wind_gust_bft_min, wind_gust_bft_max, wind_gust_bft_avg, "
        " rain) "
        " SELECT  ?, MIN(temp), MAX(temp), ROUND(AVG(temp)), "
        "         MIN(humid), MAX(humid), ROUND(AVG(humid)), "
        "         MIN(dewpoint), MAX(dewpoint), ROUND(AVG(dewpoint)), "
        "         MIN(wind), MAX(wind), ROUND(AVG(wind)), "
        "         VETERO_BEAUFORT(MIN(wind)), VETERO_BEAUFORT(MAX(wind)), VETERO_BEAUFORT(AVG(wind)), "
        "         MIN(wind_gust), MAX(wind_gust), ROUND(AVG(wind_gust)), "
        "         VETERO_BEAUFORT(MIN(wind_gust)), VETERO_BEAUFORT(MAX(wind_gust)), VETERO_BEAUFORT(AVG(wind_gust)), "
        "         SUM(rain) "
        "  FROM   weatherdata "
        "  WHERE  DATE(timestamp) = ?",
        date.c_str(), date.c_str()
    );
}

void DbAccess::updateDayStatistics()
{
    std::vector<std::string> days = dataDays(true);

    for (size_t i = 0; i < days.size(); ++i) {
        m_progressNotifier->progressed(days.size(), i);
        updateDayStatistics(days[i]);
    }

    m_progressNotifier->finished();
}

void DbAccess::updateMonthStatistics(const std::string &month)
{
    if (month.empty())
        return updateMonthStatistics();

    BW_DEBUG_INFO("Regenerating month statistics for %s", month.c_str());

    m_db->executeSql(
        "INSERT OR REPLACE INTO month_statistics "
        "(month, temp_min, temp_max, temp_avg, "
        " humid_min, humid_max, humid_avg, "
        " dewpoint_min, dewpoint_max, dewpoint_avg, "
        " wind_min, wind_max, wind_avg, "
        " wind_bft_min, wind_bft_max, wind_bft_avg, "
        " wind_gust_min, wind_gust_max, wind_gust_avg, "
        " wind_gust_bft_min, wind_gust_bft_max, wind_gust_bft_avg, "
        " rain) "
        " SELECT  ?, AVG(temp_min), AVG(temp_max), AVG(temp_avg), "
        "         AVG(humid_min), AVG(humid_max), AVG(humid_avg), "
        "         AVG(dewpoint_min), AVG(dewpoint_max), AVG(dewpoint_avg), "
        "         AVG(wind_min), AVG(wind_max), AVG(wind_avg), "
        "         VETERO_BEAUFORT(AVG(wind_min)), VETERO_BEAUFORT(AVG(wind_max)), "
        "         VETERO_BEAUFORT(AVG(wind_avg)), "
        "         AVG(wind_gust_min), AVG(wind_gust_max), AVG(wind_gust_avg), "
        "         VETERO_BEAUFORT(AVG(wind_gust_min)), VETERO_BEAUFORT(AVG(wind_gust_max)), "
        "         VETERO_BEAUFORT(AVG(wind_gust_avg)), "
        "         SUM(rain) "
        "  FROM   day_statistics "
        "  WHERE  STRFTIME('%%Y-%%m', date) = ?",
        month.c_str(), month.c_str()
    );
}

void DbAccess::updateMonthStatistics()
{
    std::vector<std::string> months = dataMonths(true);

    for (ssize_t i = 0; i < months.size(); ++i) {
        m_progressNotifier->progressed(months.size(), i);
        updateMonthStatistics(months[i]);
    }

    m_progressNotifier->finished();
}

void DbAccess::setProgressNotifier(ProgressNotifier *progress)
{
    if (progress)
        m_progressNotifier = progress;
    else
        m_progressNotifier = &dummyProgressNotifier;
}

/* }}} */

} // end namespace common
} // end namespace vetero
