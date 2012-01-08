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

sql "DROP VIEW day_statistics_float"
sql "CREATE VIEW day_statistics_float AS SELECT             \
        date                             AS date,           \
        round(temp_min/100.0, 1)         AS temp_min,       \
        round(temp_max/100.0, 1)         AS temp_max,       \
        round(temp_avg/100.0, 1)         AS temp_avg,       \
        round(humid_min/100.0, 0)        AS humid_min,      \
        round(humid_max/100.0, 0)        AS humid_max,      \
        round(humid_avg/100.0, 0)        AS humid_avg,      \
        round(dewpoint_min/100.0, 1)     AS dewpoint_min,   \
        round(dewpoint_max/100.0, 1)     AS dewpoint_max,   \
        round(dewpoint_avg/100.0, 1)     AS dewpoint_avg,   \
        round(wind_min/100.0, 1)         AS wind_min,       \
        round(wind_max/100.0, 1)         AS wind_max,       \
        round(wind_avg/100.0, 1)         AS wind_avg,       \
        wind_bft_min                     AS wind_bft_min,   \
        wind_bft_max                     AS wind_bft_max,   \
        wind_bft_avg                     AS wind_bft_avg,   \
        round(rain/1000.0, 1)            AS rain            \
    FROM day_statistics"

# update the revision
sql "UPDATE MISC set value = 4 WHERE key = 'db_revision'"

# vim: set sw=4 ts=4 et:
