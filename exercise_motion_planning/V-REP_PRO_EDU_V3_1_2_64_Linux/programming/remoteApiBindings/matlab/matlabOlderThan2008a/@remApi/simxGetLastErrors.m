function [rtn errorStrings]= simxGetLastErrors(obj,clientID,operationMode)
    errorCnt = libpointer('int32Ptr',int32(0));
    errorStrings_ = libpointer('int8PtrPtr');
    operationMode_ = int32(operationMode);

    [rtn errorCnt errorStrings_ ] = calllib(obj.libName,'simxGetLastErrors',clientID,errorCnt,errorStrings_,operationMode_);

    if (rtn==0) 
        errorStrings = cell(double(errorCnt));
        s=1;
        for i=1:errorCnt
            begin = s;
            errorStrings_.setdatatype('int8Ptr',1,s);
            value = errorStrings_.value(s);
            while(value ~= 0)
                errorStrings_.setdatatype('int8Ptr',1,s);
                value = errorStrings_.value(s);
                s=s+1;
            end
            tmp = errorStrings_.value(begin:s-1);
            errorStrings(i) = cellstr(char(tmp));
        end
    else
        errorStrings=[];
    end
end