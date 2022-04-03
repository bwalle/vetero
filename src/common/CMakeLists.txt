# {{{
# (c) 2010, Bernhard Walle <bernhard@bwalle.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>. }}}
#

set(COMMON_SRCS
    dataset.cc
    database.cc
    dbaccess.cc
    utils.cc
    error.cc
    configuration.cc
    weather.cc
    lockfile.cc
    veteroapplication.cc
    consoleprogress.cc
    httprequest.cc
    datasocket.cc
)

add_library(vetero ${COMMON_SRCS})
target_link_libraries(vetero bw)

# vim: set sw=4 ts=4 et fdm=marker:
