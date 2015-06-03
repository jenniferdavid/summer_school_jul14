function [rtn parentObjectHandle] = simxGetObjectParent(obj,clientID,objectHandle,operationMode)
    objectHandle_ = int32(objectHandle);
    parentObjectHandle = libpointer('int32Ptr',int32(0));    
    operationMode_ = int32(operationMode);

    [rtn parentObjectHandle] = calllib(obj.libName,'simxGetObjectParent',clientID,objectHandle_,parentObjectHandle,operationMode);
end