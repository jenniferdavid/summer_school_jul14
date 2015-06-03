cmake_minimum_required (VERSION 2.8)

if (EXISTS "${CMAKE_SOURCE_DIR}/cmake")
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
endif ()

find_package (FLANN)
find_package (EIGEN3) 


include_directories (
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/ext/P3P
  ${CMAKE_SOURCE_DIR}/ext
  ${FLANN_INCLUDE_DIR}
  ${EIGEN3_INCLUDE_DIR}
)

link_directories (
  ${FLANN_LIBRARY_DIR}
)

# source code directories
set (directories 
  .
  ${CMAKE_SOURCE_DIR}/ext/P3P
)


add_executable (localizer localizer.cc camera_pose.cc kd_tree_localization.cc P3P_RANSAC.cc pose_solver.cc ${CMAKE_SOURCE_DIR}/ext/P3P/P3p.cc)


target_link_libraries (localizer ${FLANN_LIBRARY})


install( PROGRAMS ${CMAKE_BINARY_DIR}/src/localizer
         DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
