# {{{
# (c) 2012, Bernhard Walle <bernhard@bwalle.de>
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

set(VETERO_DB_SRCS
    veterodb.cc
    main.cc
)

add_executable(vetero-db ${VETERO_DB_SRCS})
target_link_libraries(vetero-db ${EXTRA_LIBS} vetero)

install (TARGETS vetero-db DESTINATION bin)

# vim: set sw=4 ts=4 et fdm=marker:
