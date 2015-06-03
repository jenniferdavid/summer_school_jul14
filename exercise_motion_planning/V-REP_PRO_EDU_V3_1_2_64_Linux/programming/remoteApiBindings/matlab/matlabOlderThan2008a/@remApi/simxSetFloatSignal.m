function [rtn ]= simxSetFloatSignal(obj,clientID,signalName,signalValue,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue_ = single(signalValue);
    operationMode_ = int32(operationMode);

    [rtn signalName_ ] = calllib(obj.libName,'simxSetFloatSignal',clientID,signalName_,signalValue_,operationMode_);
end