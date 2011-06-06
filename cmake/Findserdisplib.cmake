# - Try to find serdisplib
# Once done this will define
#
#  SERDISPLIB_FOUND - system has serdisplib
#  SERDISPLIB_INCLUDE_DIRS - the serdisplib include directory
#  SERDISPLIB_LIBRARIES - Link these to use serdisplib
#  SERDISPLIB_DEFINITIONS - Compiler switches required for using serdisplib
#
#  Copyright (c) 2011 Bernhard Walle <bernhard@bwalle.de>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if (SERDISPLIB_LIBRARIES AND SERDISPLIB_INCLUDE_DIRS)
  # in cache already
  set(SERDISPLIB_FOUND TRUE)
else (SERDISPLIB_LIBRARIES AND SERDISPLIB_INCLUDE_DIRS)
  find_path(SERDISPLIB_INCLUDE_DIR
    NAMES
      serdisplib/serdisp.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
    PATH_SUFFIXES
      serdisplib
  )

  find_library(SERDISP_LIBRARY
    NAMES
      serdisp
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  mark_as_advanced(SERDISP_LIBRARY)

  if (SERDISP_LIBRARY)
    set(SERDISP_FOUND TRUE)
  endif (SERDISP_LIBRARY)

  set(SERDISPLIB_INCLUDE_DIRS
    ${SERDISPLIB_INCLUDE_DIR}
  )

  if (SERDISP_FOUND)
    set(SERDISPLIB_LIBRARIES
      ${SERDISPLIB_LIBRARIES}
      ${SERDISP_LIBRARY}
    )
  endif (SERDISP_FOUND)

  if (SERDISPLIB_INCLUDE_DIRS AND SERDISPLIB_LIBRARIES)
     set(SERDISPLIB_FOUND TRUE)
  endif (SERDISPLIB_INCLUDE_DIRS AND SERDISPLIB_LIBRARIES)

  if (SERDISPLIB_FOUND)
    if (NOT serdisplib_FIND_QUIETLY)
      message(STATUS "Found serdisplib: ${SERDISPLIB_LIBRARIES}")
    endif (NOT serdisplib_FIND_QUIETLY)
  else (SERDISPLIB_FOUND)
    if (serdisplib_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find serdisplib")
    endif (serdisplib_FIND_REQUIRED)
  endif (SERDISPLIB_FOUND)

  # show the SERDISPLIB_INCLUDE_DIRS and SERDISPLIB_LIBRARIES variables only in the advanced view
  mark_as_advanced(SERDISPLIB_INCLUDE_DIRS SERDISPLIB_INCLUDE_DIR SERDISPLIB_LIBRARIES)

endif (SERDISPLIB_LIBRARIES AND SERDISPLIB_INCLUDE_DIRS)

