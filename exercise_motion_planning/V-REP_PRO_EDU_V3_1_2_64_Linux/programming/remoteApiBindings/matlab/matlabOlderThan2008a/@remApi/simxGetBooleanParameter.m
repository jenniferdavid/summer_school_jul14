function [rtn paramValues ]= simxGetBooleanParameter(obj,clientID,paramIdentifier,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        operationMode_ = int32(operationMode);
        paramValues = libpointer('int8Ptr',int8(0));

        [rtn paramValues] = calllib(obj.libName,'simxGetBooleanParameter',clientID,paramIdentifier_,paramValues ,operationMode_);
end 