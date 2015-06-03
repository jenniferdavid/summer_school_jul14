##### Building the source
1. Build the external libraries:
  a. Go to ext/flann-1.8.4-src
  b. Create a build directory for cmake:
    mkdir build
  c. Configure the library:
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../ -DCMAKE_BUILD_TYPE=Release ..
  d. Compile and install:
    make install
    