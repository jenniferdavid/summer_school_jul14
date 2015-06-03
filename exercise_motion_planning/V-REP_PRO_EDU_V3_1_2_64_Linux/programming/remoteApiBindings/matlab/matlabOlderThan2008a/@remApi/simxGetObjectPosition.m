function [rtn position] = simxGetObjectPosition(obj,clientID,objectHandle,relativeToObjectHandle,operationMode)
    objectHandle_ = int32(objectHandle);
    relativeToObjectHandle_ = int32(relativeToObjectHandle);
    operationMode_ = int32(operationMode);
    position = libpointer('singlePtr', single([0 0 0]));

    [rtn position] = calllib(obj.libName,'simxGetObjectPosition',clientID,objectHandle_,relativeToObjectHandle_,position ,operationMode_);
end