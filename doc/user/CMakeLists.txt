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

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/Vetero.html
    COMMAND
        ${ASCIIDOC_EXECUTABLE} ${ASCIIDOC_OPTIONS}
        -o ${CMAKE_CURRENT_BINARY_DIR}/Vetero.html
        ${CMAKE_CURRENT_SOURCE_DIR}/Vetero.txt
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/Vetero.txt
    WORKING_DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(
    userdoc
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/Vetero.html
)

add_dependencies(
    veterod
    userdoc
)
