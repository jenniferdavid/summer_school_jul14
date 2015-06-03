function [rtn paramValue]= simxGetFloatingParameter(obj,clientID,paramIdentifier,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        operationMode_ = int32(operationMode);
        paramValue = libpointer('singlePtr',single(0));

        [rtn paramValue] = calllib(obj.libName,'simxGetFloatingParameter',clientID,paramIdentifier_,paramValue,operationMode_);
end 