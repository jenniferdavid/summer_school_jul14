cmake_minimum_required (VERSION 2.6)
project (xboost) 

INCLUDE(CPack)

SET(CPACK_PACKAGE_VERSION_MAJOR "2")
SET(CPACK_PACKAGE_VERSION_MINOR "3")
SET(CPACK_PACKAGE_VERSION_PATCH "0")

# if(NOT DEFINED WIN32)
#   set(Boost_USE_STATIC_LIBS   ON)
#   set(Boost_USE_MULTITHREADED ON)
#   add_definitions(-D_BOOST_MULTITHREAD)
#   find_package(Boost COMPONENTS thread)
# endif()
# 
# if ( NOT Boost_FOUND OR NOT thread_FOUND )
#   set(Boost_THREAD_LIBRARY)
# endif( NOT Boost_FOUND OR NOT thread_FOUND )

option(USE_NUMA  "Enable NUMA support"   off)

option(ENABLE_MULTITHREADING "Enable MultiThreading support"   on)

if(NOT DEFINED WIN32)
 option(USE_X11   "Enable rendering using X11"   on)
endif(NOT DEFINED WIN32)

set(EXTRA_LIBRARY)

if(USE_NUMA)
   add_definitions(-DUSE_NUMA)
   set(EXTRA_LIBRARY ${EXTRA_LIBRARY} numa)
endif(USE_NUMA)

if(NOT DEFINED WIN32 AND USE_X11)
   add_definitions(-DUSE_X11)
   set(EXTRA_LIBRARY ${EXTRA_LIBRARY} X11)
endif(NOT DEFINED WIN32 AND USE_X11)


option(USE_LINEAR_SVM  "Enable liblinear-svm support"   off)

# find_package(Boost COMPONENTS thread date_time system REQUIRED)

if(ENABLE_MULTITHREADING)
 add_definitions(-D_MULTITHREAD)
endif(ENABLE_MULTITHREADING)

include_directories(include)

add_library (xboost 
SHARED

src/timer.cpp

src/Trainer/AdaBoostTrainer.cpp
 
src/Oracle/DecisionStumpOracle.cpp 

src/Classifier/ImageClassifier.cpp

src/Preprocessor/IntegralChannelImagePreprocessor.cpp

src/Feature/HaarFeature.cpp
src/Feature/SumAreaFeature.cpp
src/Feature/IntegralChannelFeature.cpp
src/Feature/NormalizedHaarFeature.cpp 

src/FeatureGenerator/HaarFeatureGenerators.cpp
src/FeatureGenerator/IntegralChannelFeatureGenerators.cpp
# BooleanOperatorClassifier.cpp
src/IO/pnmio.cpp
src/IO/datasetin.cpp
src/IO/sdfin.cpp
src/Tools.cpp
src/Test.cpp

src/factory.cpp
src/Common.cpp
src/random.cpp
src/Types.cpp
src/ImageUtils.cpp

src/WeightedPatternResponse.cpp

src/RegisterAllClassifier.cpp


src/GUI/Window.cpp
src/GUI/x11_driver.cpp
)

target_link_libraries( xboost
# ${Boost_SYSTEM_LIBRARY}
${EXTRA_LIBRARY}
pthread
rt
)

add_executable( trainer 
src/Trainer.cpp
)

# add_executable( mtrainer 
# src/MultiClassTrainer.cpp
# )

# add_executable( svm_trainer 
# src/svm_train.cpp
# )

# 
# add_executable( svm 
# src/svm_solver.cpp
# )

# add_executable( img2haarwavelet
# src/img2haarwaveler.cpp
# )

add_executable( classify 
src/Classify.cpp
)

add_executable( ptrainer 
src/ptrainer.cpp
)


# add_executable( classify2 
# src/RegisterAllClassifier.cpp
# src/Classify2.cpp
# )
# 
# add_executable( classify3 
# src/RegisterAllClassifier.cpp
# src/Classify3.cpp
# )
# 
# add_executable( classifyx
# src/RegisterAllClassifier.cpp
# src/ClassifyX.cpp
# )
# 

add_executable( roc 
src/roc.cpp
)

add_executable( dumpfeature 
src/dumpfeature.cpp
)

add_executable( process 
src/BoostHaarTools.cpp
src/PostProcess/PostProcessFactory.cpp
src/PostProcess/CropBoost.cpp
src/PostProcess/SoftCascade.cpp
)

target_link_libraries( trainer
xboost
)

target_link_libraries( ptrainer
xboost
)

add_executable( svm_convert 
 src/svm_convert.cpp
 )

target_link_libraries( svm_convert
 xboost
 )

add_executable( svm_classify
  src/svm_classify.cpp
  )
 
target_link_libraries( svm_classify
  xboost
  )

if (USE_LINEAR_SVM) 

add_executable( svm_solver
  src/svm_solver.cpp
  )
 
target_link_libraries( svm_solver
  xboost
  linear
  )
  
endif(USE_LINEAR_SVM)

target_link_libraries( classify
xboost
rt
# ${Boost_THREAD_LIBRARY}
pthread
)

# target_link_libraries( classify2
# xboost
# rt
# ${Boost_THREAD_LIBRARY}
# pthread
# )
# 
# target_link_libraries( classify3
# xboost
# rt
# ${Boost_THREAD_LIBRARY}
# pthread
# )
# 
# target_link_libraries( classifyx
# xboost
# rt
# pthread
# ${Boost_THREAD_LIBRARY}
# )

target_link_libraries( dumpfeature
xboost
)

target_link_libraries( roc
xboost
rt
# ${Boost_THREAD_LIBRARY}
pthread
)

# target_link_libraries( img2haarwavelet
# xboost
# )

target_link_libraries( process
xboost
)

if(NOT DEFINED WIN32)
target_link_libraries (classify
rt
pthread
)

target_link_libraries (trainer
rt
# ${Boost_THREAD_LIBRARY}
pthread
)

target_link_libraries (dumpfeature
rt
pthread
)

endif(NOT DEFINED WIN32)

# option(USE_MPI  "Use MPI to build distributed trainer"   off)
# 
# if(USE_MPI)
#  find_package(MPI REQUIRED)
#  
#  set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
#  set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
#  include_directories(${MPI_INCLUDE_PATH} )
#  
#  add_executable( mpi_train 
#   src/mpi_trainer.cpp
#  )
# 
#  target_link_libraries(mpi_train xboost ${MPI_LIBRARIES}) 
#  
# endif(USE_MPI)

