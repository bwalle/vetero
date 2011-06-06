# - Try to find libusb1
# Once done this will define
#
#  LIBUSB1_FOUND - system has libusb1
#  LIBUSB1_INCLUDE_DIRS - the libusb1 include directory
#  LIBUSB1_LIBRARIES - Link these to use libusb1
#  LIBUSB1_DEFINITIONS - Compiler switches required for using libusb1
#
#  Copyright (c) 2011 Bernhard Walle <bernhard@bwalle.de>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (LIBUSB1_LIBRARIES AND LIBUSB1_INCLUDE_DIRS)
  # in cache already
  set(LIBUSB1_FOUND TRUE)
else (LIBUSB1_LIBRARIES AND LIBUSB1_INCLUDE_DIRS)
  find_path(LIBUSB1_INCLUDE_DIR
    NAMES
      libusb-1.0/libusb.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      libusb-1.0
  )

  find_library(LIBUSB-1.0_LIBRARY
    NAMES
      usb-1.0
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  mark_as_advanced(LIBUSB-1.0_LIBRARY)

  if (LIBUSB-1.0_LIBRARY)
    set(LIBUSB-1.0_FOUND TRUE)
  endif (LIBUSB-1.0_LIBRARY)

  set(LIBUSB1_INCLUDE_DIRS
    ${LIBUSB1_INCLUDE_DIR}
  )

  if (LIBUSB-1.0_FOUND)
    set(LIBUSB1_LIBRARIES
      ${LIBUSB1_LIBRARIES}
      ${LIBUSB-1.0_LIBRARY}
    )
  endif (LIBUSB-1.0_FOUND)

  if (LIBUSB1_INCLUDE_DIRS AND LIBUSB1_LIBRARIES)
     set(LIBUSB1_FOUND TRUE)
  endif (LIBUSB1_INCLUDE_DIRS AND LIBUSB1_LIBRARIES)

  if (LIBUSB1_FOUND)
    if (NOT libusb1_FIND_QUIETLY)
      message(STATUS "Found libusb1: ${LIBUSB1_LIBRARIES}")
    endif (NOT libusb1_FIND_QUIETLY)
  else (LIBUSB1_FOUND)
    if (libusb1_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find libusb1")
    endif (libusb1_FIND_REQUIRED)
  endif (LIBUSB1_FOUND)

  # show the LIBUSB1_INCLUDE_DIRS and LIBUSB1_LIBRARIES variables only in the advanced view
  mark_as_advanced(LIBUSB1_INCLUDE_DIRS LIBUSB1_INCLUDE_DIR LIBUSB1_LIBRARIES)

endif (LIBUSB1_LIBRARIES AND LIBUSB1_INCLUDE_DIRS)

