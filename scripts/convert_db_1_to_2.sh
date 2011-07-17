#!/bin/sh
#
# Updates the DB schema from version 1 to 2. This schema introduces the
# jdate (Julian date) column in TABLE weatherdata and VIEW waetherdata_float.
# This additional column is redundant since the date is also part of the timestamp,
# but it speeds up the generation of the day diagrams a lot since the DB
# doesn't have to execute strftime() on each data set.
#
# We don't use the Julian date as strict as it's defined. We use the time 12:00
# on each date applied to the timestamp strftime("%Y-%m-%d") to have the wrap
# at 00:00 localtime.

FILE=$1

if ! [ -r "$FILE" ] ; then
    echo "Usage: $0 <file>"
    exit 1
fi

sql()
{
    sqlite3 "$FILE" "$@"
}

# add the new column 'jdate' table
sql "ALTER TABLE weatherdata ADD COLUMN jdate INTEGER"

# calculate the value
sql "UPDATE weatherdata SET jdate = julianday(strftime('%Y-%m-%d 12:00', timestamp))"

# add the column to the view
sql "DROP VIEW weatherdata_float"
sql "CREATE VIEW weatherdata_float AS SELECT                \
        timestamp                        AS timestamp,      \
        jdate                            AS jdate,          \
        round(temp/100.0, 1)             AS temp,           \
        round(humid/100.0, 0)            AS humid,          \
        round(dewpoint/100.0, 1)         AS dewpoint,       \
        round(wind/100.0, 1)             AS wind,           \
        wind_bft                         AS wind_bft,       \
        round(rain/1000.0, 3)            AS rain            \
        FROM weatherdata"

# create the index
sql "CREATE INDEX index_weatherdata_jdate ON weatherdata(jdate)"

# finally add the trigger so that the jdate field is updated automatically
sql "CREATE TRIGGER update_weatherdata_jday                 \
        AFTER INSERT ON weatherdata                         \
        BEGIN                                               \
            UPDATE weatherdata                              \
            SET    jdate = julianday(strftime('%Y-%m-%d 12:00', timestamp)) \
            WHERE  timestamp = new.timestamp;               \
        END"

# update the revision
sql "UPDATE MISC set value = 2 WHERE key = 'db_revision'"

# vim: set sw=4 ts=4 et:
