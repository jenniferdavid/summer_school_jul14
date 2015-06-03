function rtn = simxAuxiliaryConsoleShow(obj,clientID,consoleHandle,showState,operationMode)
        consoleHandle_ = int32(consoleHandle);
        showState_ = int8(showState);
        operationMode_ = int32(operationMode);

        rtn = calllib(obj.libName,'simxAuxiliaryConsoleShow',clientID,consoleHandle_,showState_,operationMode_);
end  