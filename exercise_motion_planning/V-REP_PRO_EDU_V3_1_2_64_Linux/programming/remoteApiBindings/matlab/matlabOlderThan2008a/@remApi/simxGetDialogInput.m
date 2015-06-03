function [rtn inputText]= simxGetDialogInput(obj,clientID,dialogHandle,operationMode)
    dialogHandle_ = int32(dialogHandle);
    inputText_ = libpointer('int8PtrPtr');
    operationMode_ = int32(operationMode);

    [rtn inputText_ ] = calllib(obj.libName,'simxGetDialogInput',clientID,dialogHandle_,inputText_,operationMode_);

    if (rtn==0) 
        s=1;
        inputText_.setdatatype('int8Ptr',1,s);
        value = inputText_.value(s);
        while(value ~= 0)
            inputText_.setdatatype('int8Ptr',1,s);
            value = inputText_.value(s);
            s=s+1;
        end
        tmp = inputText_.value(1:s-1);
        inputText = char(tmp);
    else
        inputText = [];
    end
end