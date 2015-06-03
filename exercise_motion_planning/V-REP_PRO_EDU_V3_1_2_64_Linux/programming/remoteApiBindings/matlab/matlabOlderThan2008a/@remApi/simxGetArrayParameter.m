function [rtn paramValues ]= simxGetArrayParameter(obj,clientID,paramIdentifier,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        operationMode_ = int32(operationMode);
        paramValues = libpointer('singlePtr',single([0 0 0]));

        [rtn paramValues] = calllib(obj.libName,'simxGetArrayParameter',clientID,paramIdentifier_,paramValues ,operationMode_);
end 