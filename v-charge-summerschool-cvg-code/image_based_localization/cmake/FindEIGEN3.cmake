# - Try to find the EIGEN3 library
# Once done this will define
#  
# EIGEN3_FOUND           - system has Eigen
# EIGEN3_INCLUDE_DIR - the Eigen include directory
#   

IF (EIGEN3_INCLUDE_DIR)
 # Already in cache, be silent
 SET(EIGEN3_FIND_QUIETLY TRUE)
ENDIF (EIGEN3_INCLUDE_DIR)


FIND_PATH(EIGEN3_INCLUDE_DIR Eigen/Dense
	  PATHS "${CMAKE_SOURCE_DIR}/ext/eigen3/"
      NO_DEFAULT_PATH
                )


if( EIGEN3_INCLUDE_DIR )
   set( EIGEN3_FOUND TRUE )
ENDIF (EIGEN3_INCLUDE_DIR )