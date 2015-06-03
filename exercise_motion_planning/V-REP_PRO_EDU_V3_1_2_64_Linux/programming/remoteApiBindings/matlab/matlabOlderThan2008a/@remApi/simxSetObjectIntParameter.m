function [rtn ]= simxSetObjectIntParameter(obj,clientID,objectHandle,parameterID,parameterValue,operationMode)
    objectHandle_ = int32(objectHandle);
    parameterID_ = int32(parameterID);
    parameterValue_ = int32(parameterValue);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetObjectIntParameter',clientID,objectHandle_,parameterID_,parameterValue_,operationMode_);
end