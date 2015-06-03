function [rtn signalValue]= simxGetIntegerSignal(obj,clientID,signalName,operationMode)
        signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
        signalValue = libpointer('int32Ptr',int32(0));
        operationMode_ = int32(operationMode);

        [rtn signalName signalValue] = calllib(obj.libName,'simxGetIntegerSignal',clientID,signalName_,signalValue,operationMode_);
end 