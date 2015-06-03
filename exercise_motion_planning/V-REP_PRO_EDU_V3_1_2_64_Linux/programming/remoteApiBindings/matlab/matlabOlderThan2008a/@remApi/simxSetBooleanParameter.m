function [rtn ]= simxSetBooleanParameter(obj,clientID,paramIdentifier,paramValue,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        paramValue_ = int8(paramValue);
        operationMode_ = int32(operationMode);

        [rtn ] = calllib(obj.libName,'simxSetBooleanParameter',clientID,paramIdentifier_,paramValue_,operationMode_);
end	