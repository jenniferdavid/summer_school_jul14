function [rtn signalValue]= simxGetFloatSignal(obj,clientID,signalName,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue = libpointer('singlePtr',single(0));
    operationMode_ = int32(operationMode);

    [rtn signalName_ signalValue] = calllib(obj.libName,'simxGetFloatSignal',clientID,signalName_,signalValue,operationMode_);
end 