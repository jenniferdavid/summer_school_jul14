function rtn = simxEndDialog(obj,clientID,dialogHandle,operationMode)
        operationMode_ = int32(operationMode);
        dialogHandle_ = int32(dialogHandle);

        rtn = calllib(obj.libName,'simxEndDialog',clientID,dialogHandle_,operationMode_);
end    