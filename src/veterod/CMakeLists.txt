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

set(VETEROD_SRCS
    veterod.h
    veterod.cc
    datareader.cc
    childprocesswatcher.cc
    clouduploader.cc
    main.cc
)

add_executable(veterod ${VETEROD_SRCS})
target_link_libraries(veterod ${EXTRA_LIBS} vetero)

install (TARGETS veterod DESTINATION bin)

# vim: set sw=4 ts=4 et fdm=marker:
