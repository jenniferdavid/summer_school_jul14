cmake_minimum_required (VERSION 2.8)

project (ImageBasedLocalization)

# add our macro directory to cmake search path
if (EXISTS "${CMAKE_SOURCE_DIR}/cmake")
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
endif ()

set( CMAKE_DEBUG_POSTFIX "d" )


add_subdirectory (src)