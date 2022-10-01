# Database schema

## Table `weatherdata`

This table contains all data sets that are received.

| Name              | Type       | Description                                             |
| ----------------- | ---------- | ------------------------------------------------------- |
| `timestamp`       | `DATETIME` | Time of the measurement, localtime                      |
| `jdate`           | `INTEGER`  | Julian date (redundant but improves performance)        |
| `temp`            | `INTEGER`  | Temperature in 1/100 °C (e.g. 2750)                     |
| `humid`           | `INTEGER`  | Humidity in 1/100 % (e.g. 7500). 0 if the sensor doesn't measure humidity. |
| `dewpoint`        | `INTEGER`  | Dewpoint in 1/100 °C (e.g. 1675). Calculcated from the temperature and the humidity. 0 if the sensor doesn't measure the humidity. |
| `wind`            | `INTEGER`  | Wind in 1/100 km/h (e.g. 400). 0 if the sensor doesn't measure the wind. |
| `wind_bft`        | `INTEGER`  | Wind strength in Beaufort.                              |
| `wind_gust`       | `INTEGER`  | Wind gust in 1/100 km/h (e.g. 400). 0 if the sensor doesn't measure the wind. |
| `wind_gust_bft`   | `INTEGER`  | Wind strength in Beaufort.                              |
| `rain`            | `INTEGER`  | Rain in 1/1000 l/m², counted from the last measurement. |
| `solar_radiation` | `INTEGER`  | Solar radiation in W/m².                                |
| `uv_index`        | `INTEGER`  | UV Index.                                               |
| `pressure`        | `INTEGER`  | Pressure at sea level in 1/100 hPa.                     |
| `solar_radiation` | `INTEGER`  | Solar radiation in 1/10 W/m².                           |

## Table `day_statistics`

This table contains the statistics for one day and is updated on every measurement.

| Name                | Type       | Description                                         |
| ------------------- | ---------- | --------------------------------------------------- |
| `date`              | `DATE`     | Date of the statistics.                             |
| `temp_min`          | `INTEGER`  | Minimum temperature in 1/100 °C.                    |
| `temp_max`          | `INTEGER`  | Maximum temperature in 1/100 °C.                    |
| `temp_avg`          | `INTEGER`  | Average temperature in 1/100 °C.                    |
| `humid_min`         | `INTEGER`  | Minimum humidity in 1/100 %.                        |
| `humid_max`         | `INTEGER`  | Maximum humidity in 1/100 %.                        |
| `humid_avg`         | `INTEGER`  | Average humidity in 1/100 %.                        |
| `dewpoint_min`      | `INTEGER`  | Minimum dewpoint in 1/100 °C.                       |
| `dewpoint_max`      | `INTEGER`  | Maximum dewpoint in 1/100 °C.                       |
| `dewpoint_avg`      | `INTEGER`  | Average dewpoint in 1/100 °C.                       |
| `wind_min`          | `INTEGER`  | Minimum wind in 1/100 km/h.                         |
| `wind_max`          | `INTEGER`  | Maximum wind in 1/100 km/h.                         |
| `wind_avg`          | `INTEGER`  | Average wind in 1/100 km/h.                         |
| `wind_bft_min`      | `INTEGER`  | Minimum wind strength in Beaufort.                  |
| `wind_bft_max`      | `INTEGER`  | Maximum wind strength in Beaufort.                  |
| `wind_bft_avg`      | `INTEGER`  | Average wind strength in Beaufort.                  | 
| `wind_gust_min`     | `INTEGER`  | Minimum wind gust in 1/100 km/h.                    |
| `wind_gust_max`     | `INTEGER`  | Maximum wind gust in 1/100 km/h.                    |
| `wind_gust_avg`     | `INTEGER`  | Average wind gust in 1/100 km/h.                    |
| `wind_gust_bft_min` | `INTEGER`  | Minimum wind gust strength in Beaufort.             |
| `wind_gust_bft_max` | `INTEGER`  | Maximum wind gust strength in Beaufort.             |
| `wind_gust_bft_avg` | `INTEGER`  | Average wind gust strength in Beaufort.             | 
| `rain`              | `INTEGER`  | Total rain of the month in 1/1000 l/m².             |


## Table `month_statistics`

| Name            | Type       | Description                                         |
| --------------- | ---------- | --------------------------------------------------- |
| `month`         | `STRING`   | Month of the statistics as YYYY-MM string.          |
| `temp_min`      | `INTEGER`  | Minimum temperature in 1/100 °C.                    |
| `temp_max`      | `INTEGER`  | Maximum temperature in 1/100 °C.                    |
| `temp_avg`      | `INTEGER`  | Average temperature in 1/100 °C.                    |
| `humid_min`     | `INTEGER`  | Minimum humidity in 1/100 %.                        |
| `humid_max`     | `INTEGER`  | Maximum humidity in 1/100 %.                        |
| `humid_avg`     | `INTEGER`  | Average humidity in 1/100 %.                        |
| `dewpoint_min`  | `INTEGER`  | Minimum dewpoint in 1/100 °C.                       |
| `dewpoint_max`  | `INTEGER`  | Maximum dewpoint in 1/100 °C.                       |
| `dewpoint_avg`  | `INTEGER`  | Average dewpoint in 1/100 °C.                       |
| `wind_min`      | `INTEGER`  | Minimum wind gust in 1/100 km/h.                    |
| `wind_max`      | `INTEGER`  | Maximum wind gust in 1/100 km/h.                    |
| `wind_avg`      | `INTEGER`  | Average wind gust in 1/100 km/h.                    |
| `wind_bft_min`  | `INTEGER`  | Minimum wind gust strength in Beaufort.             |
| `wind_bft_max`  | `INTEGER`  | Maximum wind gust strength in Beaufort.             |
| `wind_bft_avg`  | `INTEGER`  | Average wind gust strength in Beaufort.             |
| `rain`          | `INTEGER`  | Total rain of the month in 1/1000 l/m².             |
