function [rtn ]= simxSetStringSignal(obj,clientID,signalName,signalValue,operationMode)
    signalLength_ = int32(length(signalValue));
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue_ = libpointer('int8Ptr',[int8(signalValue) 0]);
    operationMode_ = int32(operationMode);

    [rtn signalName_ signalValue_ ] = calllib(obj.libName,'simxSetStringSignal',clientID,signalName_,signalValue_,signalLength_ ,operationMode_);
end