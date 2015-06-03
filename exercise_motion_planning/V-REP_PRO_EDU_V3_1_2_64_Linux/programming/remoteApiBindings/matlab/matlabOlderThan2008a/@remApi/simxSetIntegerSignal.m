function [rtn ]= simxSetIntegerSignal(obj,clientID,signalName,signalValue,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue_ = int32(signalValue);
    operationMode_ = int32(operationMode);

    [rtn signalName_ ] = calllib(obj.libName,'simxSetIntegerSignal',clientID,signalName_,signalValue_,operationMode_);
end