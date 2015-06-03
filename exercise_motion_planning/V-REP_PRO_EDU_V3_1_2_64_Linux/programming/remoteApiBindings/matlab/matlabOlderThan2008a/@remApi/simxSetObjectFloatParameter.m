function [rtn ]= simxSetObjectFloatParameter(obj,clientID,objectHandle,parameterID,parameterValue,operationMode)
    objectHandle_ = int32(objectHandle);
    parameterID_ = int32(parameterID);
    parameterValue_ = single(parameterValue);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetObjectFloatParameter',clientID,objectHandle_,parameterID_,parameterValue_,operationMode_);
end