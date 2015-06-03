function [rtn result]= simxGetDialogResult (obj,clientID,dialogHandle,operationMode)
        dialogHandle_  = int32(dialogHandle);
        result = libpointer('int32Ptr',int32(0));
        operationMode_ = int32(operationMode);

        [rtn result] = calllib(obj.libName,'simxGetDialogResult',clientID,dialogHandle_,result,operationMode_);
end 