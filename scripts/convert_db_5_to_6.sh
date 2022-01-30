#!/bin/sh
#
# Updates the DB schema from version 2 to 3. This schema introduces the air pressure.
# field in table weatherdata and in the view weatherdata_float.

FILE=$1

if ! [ -r "$FILE" ] ; then
    echo "Usage: $0 <file>"
    exit 1
fi

sql()
{
    sqlite3 "$FILE" "$@"
}

echo "ALTER weatherdata"
sql "ALTER TABLE weatherdata ADD COLUMN wind_gust INTEGER"
sql "ALTER TABLE weatherdata ADD COLUMN wind_gust_bft INTEGER"

echo "ALTER day_statistics"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_min INTEGER"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_max INTEGER"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_avg INTEGER"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_bft_min INTEGER"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_bft_max INTEGER"
sql "ALTER TABLE day_statistics ADD COLUMN wind_gust_bft_avg INTEGER"

echo "ALTER month_statistics"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_min INTEGER"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_max INTEGER"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_avg INTEGER"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_bft_min INTEGER"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_bft_max INTEGER"
sql "ALTER TABLE month_statistics ADD COLUMN wind_gust_bft_avg INTEGER"

echo "RE-CREATE weatherdata_float"
sql "DROP VIEW weatherdata_float"
sql "CREATE VIEW weatherdata_float AS SELECT                        \
                timestamp                     AS timestamp,         \
                jdate                            AS jdate,          \
                round(temp/100.0, 1)             AS temp,           \
                round(humid/100.0, 0)            AS humid,          \
                round(dewpoint/100.0, 1)         AS dewpoint,       \
                round(wind/100.0, 1)             AS wind,           \
                wind_bft                         AS wind_bft,       \
                round(wind_gust/100.0, 1)        AS wind_gust,      \
                wind_gust_bft                    AS wind_gust_bft,  \
                wind_dir                         AS wind_dir,       \
                round(rain/1000.0, 3)            AS rain,           \
                round(pressure/100.0, 0)         AS pressure        \
        FROM weatherdata"

echo "RE-CREATE day_statistics_float"
sql "DROP VIEW day_statistics_float"
sql "CREATE VIEW day_statistics_float AS SELECT                     \
            date                             AS date,               \
            round(temp_min/100.0, 1)         AS temp_min,           \
            round(temp_max/100.0, 1)         AS temp_max,           \
            round(temp_avg/100.0, 1)         AS temp_avg,           \
            round(humid_min/100.0, 0)        AS humid_min,          \
            round(humid_max/100.0, 0)        AS humid_max,          \
            round(humid_avg/100.0, 0)        AS humid_avg,          \
            round(dewpoint_min/100.0, 1)     AS dewpoint_min,       \
            round(dewpoint_max/100.0, 1)     AS dewpoint_max,       \
            round(dewpoint_avg/100.0, 1)     AS dewpoint_avg,       \
            round(wind_min/100.0, 1)         AS wind_min,           \
            round(wind_max/100.0, 1)         AS wind_max,           \
            round(wind_avg/100.0, 1)         AS wind_avg,           \
            wind_bft_min                     AS wind_bft_min,       \
            wind_bft_max                     AS wind_bft_max,       \
            wind_bft_avg                     AS wind_bft_avg,       \
            round(wind_gust_min/100.0, 1)    AS wind_gust_min,      \
            round(wind_gust_max/100.0, 1)    AS wind_gust_max,      \
            round(wind_gust_avg/100.0, 1)    AS wind_gust_avg,      \
            wind_gust_bft_min                AS wind_gust_bft_min,  \
            wind_gust_bft_max                AS wind_gust_bft_max,  \
            wind_gust_bft_avg                AS wind_gust_bft_avg,  \
            round(rain/1000.0, 1)            AS rain                \
        FROM day_statistics"

echo "RE-CREATE month_statistics_float"
sql "DROP VIEW month_statistics_float"
sql "CREATE VIEW month_statistics_float AS SELECT                       \
            month                                AS month,              \
            round(temp_min/100.0, 1)             AS temp_min,           \
            round(temp_max/100.0, 1)             AS temp_max,           \
            round(temp_avg/100.0, 1)             AS temp_avg,           \
            round(humid_min/100.0, 0)            AS humid_min,          \
            round(humid_max/100.0, 0)            AS humid_max,          \
            round(humid_avg/100.0, 0)            AS humid_avg,          \
            round(dewpoint_min/100.0, 1)         AS dewpoint_min,       \
            round(dewpoint_max/100.0, 1)         AS dewpoint_max,       \
            round(dewpoint_avg/100.0, 1)         AS dewpoint_avg,       \
            round(wind_min/100.0, 1)             AS wind_min,           \
            round(wind_max/100.0, 1)             AS wind_max,           \
            round(wind_avg/100.0, 1)             AS wind_avg,           \
            round(wind_bft_min/1000.0, 1)        AS wind_bft_min,       \
            round(wind_bft_max/1000.0, 1)        AS wind_bft_max,       \
            round(wind_bft_avg/1000.0, 1)        AS wind_bft_avg,       \
            round(wind_gust_min/100.0, 1)        AS wind_gust_min,      \
            round(wind_gust_max/100.0, 1)        AS wind_gust_max,      \
            round(wind_gust_avg/100.0, 1)        AS wind_gust_avg,      \
            round(wind_gust_bft_min/1000.0, 1)   AS wind_gust_bft_min,  \
            round(wind_gust_bft_max/1000.0, 1)   AS wind_gust_bft_max,  \
            round(wind_gust_bft_avg/1000.0, 1)   AS wind_gust_bft_avg,  \
            round(rain/1000.0, 1)            AS rain                    \
        FROM month_statistics"


# update the revision
sql "UPDATE MISC set value = 6 WHERE key = 'db_revision'"

# vim: set sw=4 ts=4 et:
