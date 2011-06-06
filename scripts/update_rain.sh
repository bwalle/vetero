#!/bin/bash

SQLITE_DB=$1

if [ -z "$SQLITE_DB" ] ; then
    echo "Usage: $0 <sqlite3_db>"
    exit 1
fi

for date in $(sqlite3 "$SQLITE_DB" 'select distinct date(timestamp) from weatherdata;' ) ; do
   sqlite3 "$SQLITE_DB" \
           "INSERT OR REPLACE INTO rain_min (date, min_rain) VALUES ( date('$date'), \
           ( SELECT min(rain_gauge) FROM weatherdata  WHERE  date(timestamp) = date('$date') ) )"
done
