function [rtn paramValue]= simxGetIntegerParameter (obj,clientID,paramIdentifier,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        operationMode_ = int32(operationMode);
        paramValue = libpointer('int32Ptr',int32(0));

        [rtn paramValue] = calllib(obj.libName,'simxGetIntegerParameter',clientID,paramIdentifier_,paramValue,operationMode_);
end 