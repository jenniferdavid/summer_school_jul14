# Import the numpy to Eigen type conversion.
import os

isCompiled = False
pathToSo = os.path.dirname(os.path.realpath(__file__))
if os.path.isfile(os.path.join(pathToSo,"libcommon_planning_types_python.so")):
    # Import the the C++ exports from your package library.
    from libcommon_planning_types_python import *
    # Import other files in the directory
    # from mypyfile import *
    isCompiled = True
else:
    print "Warning: the package libcommon_planning_types_python is not compiled. Type 'rosmake libcommon_planning_types_python' if you need this."
    PACKAGE_IS_NOT_COMPILED = True;


