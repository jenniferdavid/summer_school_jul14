function [rtn paramValue]= simxGetObjectIntParameter(obj,clientID,objectHandle,parameterID,operationMode)
    objectHandle_ = int32(objectHandle);
    parameterID_ = int32(parameterID);
    operationMode_ = int32(operationMode);
    paramValue = libpointer('int32Ptr',int32(0));

    [rtn paramValue] = calllib(obj.libName,'simxGetObjectIntParameter',clientID,objectHandle_,parameterID_,paramValue,operationMode_);
end