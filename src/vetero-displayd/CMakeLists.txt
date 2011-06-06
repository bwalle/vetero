# {{{
# (c) 2011, Bernhard Walle <bernhard@bwalle.de>
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

set(VETERO_DISPLAYD_SRCS
    main.cc
    serdisplibdisplay.cc
    charset.cc
    vetero_displayd.cc
)

add_executable(vetero-displayd ${VETERO_DISPLAYD_SRCS})
target_link_libraries(vetero-displayd ${EXTRA_LIBS} ${EXTRA_LCD_LIBS} vetero bw)

install (TARGETS vetero-displayd DESTINATION bin)

# vim: set sw=4 ts=4 et fdm=marker:
