import numpy_eigen
from common_planning_types_python import *
import os

isCompiled = False
pathToSo = os.path.dirname(os.path.realpath(__file__))
if os.path.isfile(os.path.join(pathToSo,"liblocal_planning_python.so")):
    from liblocal_planning_python import *
    isCompiled = True
else:
    print "Warning: the package liblocal_planning_python is not compiled."
    PACKAGE_IS_NOT_COMPILED = True;


