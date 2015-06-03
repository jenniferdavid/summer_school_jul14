function [rtn eulerAngles] = simxGetObjectOrientation(obj,clientID,objectHandle,relativeToObjectHandle,operationMode)
    objectHandle_ = int32(objectHandle);
    relativeToObjectHandle_ = int32(relativeToObjectHandle);
    operationMode_ = int32(operationMode);
    eulerAngles = libpointer('singlePtr', single([0 0 0]));

    [rtn eulerAngles] = calllib(obj.libName,'simxGetObjectOrientation',clientID,objectHandle_,relativeToObjectHandle_,eulerAngles ,operationMode_);
end