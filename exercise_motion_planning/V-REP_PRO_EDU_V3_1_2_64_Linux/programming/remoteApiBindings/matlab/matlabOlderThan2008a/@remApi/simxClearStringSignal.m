function rtn = simxClearStringSignal(obj,clientID,signalName,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    operationMode_ = int32(operationMode);

    rtn = calllib(obj.libName,'simxClearStringSignal',clientID,signalName_,operationMode_);
end