function [rtn paramValue]= simxGetObjectFloatParameter(obj,clientID,objectHandle,parameterID,operationMode)
    objectHandle_ = int32(objectHandle);
    parameterID_ = int32(parameterID);
    operationMode_ = int32(operationMode);
    paramValue = libpointer('singlePtr',single(0));

    [rtn paramValue] = calllib(obj.libName,'simxGetObjectFloatParameter',clientID,objectHandle_,parameterID_,paramValue,operationMode_);
end