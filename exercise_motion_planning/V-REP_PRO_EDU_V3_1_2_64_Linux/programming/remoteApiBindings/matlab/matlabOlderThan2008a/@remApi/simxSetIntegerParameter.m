function [rtn ]= simxSetIntegerParameter(obj,clientID,paramIdentifier,paramValue,operationMode)
    paramIdentifier_ = int32(paramIdentifier);
    paramValue_ = int32(paramValue);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetIntegerParameter',clientID,paramIdentifier_,paramValue_,operationMode_);
end