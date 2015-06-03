function [rtn ] = simxRemoveUI(obj,clientID,uiHandle,operationMode)
        uiHandle_ = int32(uiHandle);
        operationMode_ = int32(operationMode);

        [rtn ]  = calllib(obj.libName,'simxRemoveUI',clientID,uiHandle_,operationMode_);
end	