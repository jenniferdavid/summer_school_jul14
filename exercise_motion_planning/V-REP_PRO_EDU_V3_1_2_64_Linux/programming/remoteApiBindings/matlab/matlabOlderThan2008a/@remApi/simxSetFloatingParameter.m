function [rtn ]= simxSetFloatingParameter(obj,clientID,paramIdentifier,paramValue,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        paramValue_ = single(paramValue);
        operationMode_ = int32(operationMode);

        [rtn ] = calllib(obj.libName,'simxSetFloatingParameter',clientID,paramIdentifier_,paramValue_,operationMode_);
end  