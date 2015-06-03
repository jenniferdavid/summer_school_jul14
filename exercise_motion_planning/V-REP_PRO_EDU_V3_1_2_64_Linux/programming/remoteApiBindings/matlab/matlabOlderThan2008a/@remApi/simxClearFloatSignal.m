function rtn = simxClearFloatSignal(obj,clientID,signalName,operationMode)
        signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
        operationMode_ = int32(operationMode);

        rtn = calllib(obj.libName,'simxClearFloatSignal',clientID,signalName_,operationMode_);
end  