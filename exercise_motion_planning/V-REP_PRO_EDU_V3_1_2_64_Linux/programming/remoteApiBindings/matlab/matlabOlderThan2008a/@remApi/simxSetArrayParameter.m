function [rtn ]= simxSetArrayParameter(obj,clientID,paramIdentifier,paramValues,operationMode)
        paramIdentifier_ = int32(paramIdentifier);
        num_ele = numel(paramValues);
        if (num_ele < 3)
            error('paramValues should have 3 values');
        else
            paramValues_ = libpointer('singlePtr',single(paramValues));
            operationMode_ = int32(operationMode);

            [rtn paramValues_ ] = calllib(obj.libName,'simxSetArrayParameter',clientID,paramIdentifier_,paramValues_,operationMode_);
        end
end	