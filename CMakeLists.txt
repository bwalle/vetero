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

project(vetero C CXX)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

#
# doxygen target
#
add_custom_target(
    apidoc
    doxygen ${CMAKE_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API documentation"
)

#
# tags target
#
add_custom_target(
    tags
    ctags -R .
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating tags database"
)

#
# sqlite
#

include(FindPkgConfig)
pkg_check_modules(SQLITE3 REQUIRED "sqlite3")
if (NOT SQLITE3_FOUND)
    message(FATAL_ERROR "sqlite3 C/C++ library could not be found")
endif (NOT SQLITE3_FOUND)

include_directories(${SQLITE3_INCLUDE_DIRS})
find_library(SQLITE3_LIBRARIES_FULL ${SQLITE3_LIBRARIES})
set(EXTRA_LIBS ${EXTRA_LIBS} ${SQLITE3_LIBRARIES_FULL})

#
# libintl
#

include(FindLibintl)
if (NOT LIBINTL_FOUND)
    message(FATAL_ERROR "libintl library could not be found")
endif (NOT LIBINTL_FOUND)
include_directories(${LIBINTL_INCLUDE_DIR})
set(EXTRA_LIBS ${EXTRA_LIBS} ${LIBINTL_LIBRARIES})


#
# libconfuse
#

pkg_check_modules(CONFUSE REQUIRED "libconfuse")
if (NOT CONFUSE_FOUND)
    message(FATAL_ERROR "confuse library could not be found")
endif (NOT CONFUSE_FOUND)
include_directories(${CONFUSE_INCLUDE_DIRS})

# fully trust pkg-config is more safe when using it for native
# builds, but fails when we cross-compile for Linux.
if (CMAKE_CROSSCOMPILING)
    find_library(CONFUSE_LIBRARIES_FULL ${CONFUSE_LIBRARIES})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CONFUSE_LIBRARIES_FULL})
else (CMAKE_CROSSCOMPILING)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CONFUSE_LDFLAGS})
endif (CMAKE_CROSSCOMPILING)

#
# zlib
#

include(FindZLIB)
if (NOT ZLIB_FOUND)
    message(FATAL_ERROR "zlib library could not be found")
endif (NOT ZLIB_FOUND)
include_directories(${ZLIB_INCLUDE_DIRS})
set(EXTRA_LIBS ${EXTRA_LIBS} ${ZLIB_LIBRARIES})


#
# curl
#


find_package(CURL)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${CURL_LIBRARY})

    message(STATUS "CURL library found")
else (CURL_FOUND)
    message(FATAL_ERROR "curl library not found")
endif (CURL_FOUND)



#
# serdisplib
#

include(Findserdisplib)
if (NOT SERDISPLIB_FOUND)
    message(WARNING "Compiling vetero without display support")
    set(BUILD_VETERO_DISPLAYD FALSE)
else (NOT SERDISPLIB_FOUND)
    message(STATUS "Compiling vetero with display support")
    set(BUILD_VETERO_DISPLAYD TRUE)

    include_directories(${SERDISPLIB_INCLUDE_DIRS})
    set(EXTRA_LCD_LIBS ${SERDISPLIB_LIBRARIES})
endif (NOT SERDISPLIB_FOUND)


#
# libusb
#

include(Findlibusb)
if (LIBUSB_FOUND)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${LIBUSB_LIBRARIES})
    set(LIBUSB_VERSION "0.1")
endif (LIBUSB_FOUND)

include (Findlibusb1)
if (LIBUSB1_FOUND)
    include_directories(${LIBUSB1_INCLUDE_DIRS})
    set(EXTRA_LIBS ${EXTRA_LIBS} ${LIBUSB1_LIBRARIES})
    set(LIBUSB_VERSION "1.0")
endif (LIBUSB1_FOUND)

#
# rapidJSON
#

find_package(RapidJSON 1.1 REQUIRED MODULE)
if (NOT RAPIDJSON_FOUND)
    message(FATAL_ERROR "RapidJSON not found")
endif (NOT RAPIDJSON_FOUND)


#
# libbw
#

include_directories(${vetero_SOURCE_DIR}/ext/libbw.git)
add_subdirectory(ext/libbw.git/libbw)
set (EXTRA_LIBS ${EXTRA_LIBS} bw)

#
# usbpp
#

include_directories(${vetero_SOURCE_DIR}/ext/usbpp.git)
add_subdirectory(ext/usbpp.git/usbpp)
set (EXTRA_LIBS ${EXTRA_LIBS} usbpp)

#
# git version
#
execute_process(COMMAND           git describe --tags
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE   GIT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

#
# Configure file
#

configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

#
# Translations
#

include(FindGettext)
if (NOT GETTEXT_FOUND)
    message(WARNING "GNU gettext tools not found")
endif (NOT GETTEXT_FOUND)
set(VETERO_PO_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
add_custom_target(pot
    DEPENDS vetero-reportgen-pot
    DEPENDS vetero-displayd-pot)


add_subdirectory(src)
add_subdirectory(share)
add_subdirectory(po)

include(CPack)

# vim: set sw=4 ts=4 et fdm=marker:
