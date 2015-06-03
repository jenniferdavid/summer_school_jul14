import numpy_eigen
import os

isCompiled = False
pathToSo = os.path.dirname(os.path.realpath(__file__))
if os.path.isfile(os.path.join(pathToSo,"libbt_transform_python.so")):
    from libbt_transform_python import *
    isCompiled = True
else:
    print "Warning: the package libbt_transform_python is not compiled."
    PACKAGE_IS_NOT_COMPILED = True;


