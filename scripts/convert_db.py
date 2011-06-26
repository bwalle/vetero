#!/usr/bin/python3
#
# Converts a DB from the old view-based format to the new format which performs
# more caching.
#
# The new DB must already exist (i.e. veterod must have been called once to
# create the DB). After the conversion, 'veterod -R' must be called to generate
# the metadata.

import sqlite3
import sys
import math

def bft(kmh):
    kmh = round(kmh)
    if (kmh < 1):
        return 0
    elif (kmh <= 5):
        return 1
    elif (kmh <= 11):
        return 2
    elif (kmh <= 19):
        return 3
    elif (kmh <= 28):
        return 4
    elif (kmh <= 38):
        return 5
    elif (kmh <= 49):
        return 6
    elif (kmh <= 61):
        return 7
    elif (kmh <= 74):
        return 8
    elif (kmh <= 88):
        return 9
    elif (kmh <= 102):
        return 10
    elif (kmh <= 117):
        return 11
    else:
        return 12

def dewpoint(temp, humid):
    return (241.2 * math.log(humid/100.0) + ((4222.03716*temp)/(241.2+temp))) / \
            (17.5043 - math.log(humid/100.0) - ((17.5043*temp)/(241.2+temp)))

if len(sys.argv) != 3:
    sys.exit('Usage: convert_db.py <olddb> <newdb>')

old_conn = sqlite3.connect(sys.argv[2])
old_cursor = old_conn.cursor()

old_cursor.execute(
    """SELECT   timestamp, temp, humid, rain_gauge, is_rain, wind
       FROM     weatherdata
       ORDER BY timestamp""")

new_conn = sqlite3.connect(sys.argv[1])
new_cursor = new_conn.cursor()

new_cursor.execute(
    """DELETE FROM weatherdata"""
)

old_gauge = -1
for result in old_cursor:
    timestamp = result[0]
    temp = float(result[1])
    humid = float(result[2])
    rain_gauge = int(result[3])
    wind = float(result[5])

    if old_gauge == -1:
        old_gauge = rain_gauge
    rain = rain_gauge - old_gauge
    old_gauge = rain_gauge

    new_cursor.execute(
        """INSERT INTO weatherdata
           VALUES (?,?,?,?,?,?,?)""",
        [ timestamp,
          round(temp*100.0),
          round(humid*100.0),
          round(dewpoint(temp, humid)*100),
          round(wind*100.0),
          bft(wind),
          round(rain * 295) ]
    )

new_cursor.execute(
    """INSERT OR REPLACE INTO misc
       VALUES ('last_rain',?)""",
    [old_gauge]
)

new_conn.commit()

# vim: set sw=4 ts=4 et:
