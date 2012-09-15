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

set(VETERO_REPORTGEN_SRCS
    htmldocument.cc
    gnuplot.cc
    vetero_reportgen.cc
    reportgenerator.cc
    dayreportgenerator.cc
    currentreportgenerator.cc
    monthreportgenerator.cc
    yearreportgenerator.cc
    indexgenerator.cc
    main.cc
    calendar.cc
    validdatacache.cc
    nameprovider.cc
)

add_executable(vetero-reportgen ${VETERO_REPORTGEN_SRCS})
target_link_libraries(vetero-reportgen ${EXTRA_LIBS} vetero bw)

install (TARGETS vetero-reportgen DESTINATION bin)

# Translation
add_custom_target(vetero-reportgen-pot
    COMMAND             xgettext -c++ --from-code=utf-8 -k_
                        -o ${VETERO_PO_DIRECTORY}/vetero-reportgen/vetero-reportgen.pot
                        ${VETERO_REPORTGEN_SRCS}
    WORKING_DIRECTORY   ${CMAKE_CURRENT_SOURCE_DIR}
)

# vim: set sw=4 ts=4 et fdm=marker:
