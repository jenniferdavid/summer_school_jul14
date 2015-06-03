function [rtn ]= simxSetModelProperty(obj,clientID,objectHandle,prop,operationMode)
    objectHandle_ = int32(objectHandle);
    prop_ = int32(prop);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetModelProperty',clientID,objectHandle_,prop_,operationMode_);
end