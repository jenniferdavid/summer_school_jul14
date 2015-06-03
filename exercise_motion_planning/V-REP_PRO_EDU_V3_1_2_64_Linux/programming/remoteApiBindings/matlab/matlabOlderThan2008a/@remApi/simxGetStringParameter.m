function [rtn paramValue]= simxGetStringParameter(obj,clientID,paramIdentifier,operationMode)
    paramIdentifier_ = int32(paramIdentifier);
    operationMode_ = int32(operationMode);
    paramValue_ = libpointer('int8PtrPtr');

    [rtn paramValue_] = calllib(obj.libName,'simxGetStringParameter',clientID,paramIdentifier_,paramValue_,operationMode_);

    if(rtn == 0)
        s=1;
        paramValue_.setdatatype('int8Ptr',1,s);
        value = paramValue_.value(s);
        while(value ~= 0)
            paramValue_.setdatatype('int8Ptr',1,s);
            value = paramValue_.value(s);
            s=s+1;
        end
        tmp = paramValue_.value(1:s-1);
        paramValue = char(tmp);
    else
        paramValue = [];
    end
end