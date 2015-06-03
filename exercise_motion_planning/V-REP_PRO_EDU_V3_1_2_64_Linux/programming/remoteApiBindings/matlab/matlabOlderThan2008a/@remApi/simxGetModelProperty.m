function [rtn prop] = simxGetModelProperty(obj,clientID,objectHandle,operationMode)
    objectHandle_ = int32(objectHandle);
    operationMode_ = int32(operationMode);
    prop = libpointer('int32Ptr',int32(0));

    [rtn prop ] = calllib(obj.libName,'simxGetModelProperty',clientID,objectHandle_,prop,operationMode_);
end