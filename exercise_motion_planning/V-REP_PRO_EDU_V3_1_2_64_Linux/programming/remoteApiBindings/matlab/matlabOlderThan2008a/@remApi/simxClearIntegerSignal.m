function rtn = simxClearIntegerSignal(obj,clientID,signalName,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    operationMode_ = int32(operationMode);

    rtn = calllib(obj.libName,'simxClearIntegerSignal',clientID,signalName_,operationMode_);
end