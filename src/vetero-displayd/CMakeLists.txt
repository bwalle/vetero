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

if (BUILD_VETERO_DISPLAYD)
    add_executable(vetero-displayd ${VETERO_DISPLAYD_SRCS})
    target_link_libraries(vetero-displayd ${EXTRA_LIBS} ${EXTRA_LCD_LIBS} vetero bw)

    install (TARGETS vetero-displayd DESTINATION bin)
endif (BUILD_VETERO_DISPLAYD)

# Translation
add_custom_target(vetero-displayd-pot
    COMMAND             xgettext -c++ --from-code=utf-8 -k_
                        -o ${VETERO_PO_DIRECTORY}/vetero-displayd/vetero-displayd.pot
                        ${VETERO_DISPLAYD_SRCS}
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
)

# vim: set sw=4 ts=4 et fdm=marker:
