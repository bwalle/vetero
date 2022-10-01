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
sql "ALTER TABLE weatherdata ADD COLUMN solar_radiation INTEGER"
sql "ALTER TABLE weatherdata ADD COLUMN uv_index INTEGER"

echo "RE-CREATE weatherdata_float"
sql "DROP VIEW weatherdata_float"
sql "CREATE VIEW weatherdata_float AS SELECT                        \
                timestamp                        AS timestamp,      \
                jdate                            AS jdate,          \
                round(temp/100.0, 1)             AS temp,           \
                round(humid/100.0, 0)            AS humid,          \
                round(dewpoint/100.0, 1)         AS dewpoint,       \
                round(wind/100.0, 1)             AS wind,           \
                wind_bft                         AS wind_bft,       \
                round(wind_gust/100.0, 1)        AS wind_gust,      \
                wind_gust_bft                    AS wind_gust_bft,  \
                wind_dir                         AS wind_dir,       \
                solar_radiation                  AS solar_radiation,\
                uv_index                         AS uv_index,       \
                round(rain/1000.0, 3)            AS rain,           \
                round(pressure/100.0, 0)         AS pressure        \
        FROM weatherdata"

# update the revision
sql "UPDATE MISC set value = 7 WHERE key = 'db_revision'"

# vim: set sw=4 ts=4 et:
