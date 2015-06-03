function [rtn] = simxSetObjectParent(obj,clientID,objectHandle,parentObject,keepInPlace,operationMode)
    objectHandle_ = int32(objectHandle);
    parentObject_ = int32(parentObject);
    keepInPlace_ = int8(keepInPlace);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetObjectParent',clientID,objectHandle_ ,parentObject_,keepInPlace_,operationMode_);
end