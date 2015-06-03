function [rtn signalValue ]= simxGetAndClearStringSignal(obj,clientID,signalName,operationMode)
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue_ = libpointer('int8PtrPtr');
    signalLength= libpointer('int32Ptr',int32(0));
    operationMode_ = int32(operationMode);

    [rtn signalName_ signalValue_ signalLength ] = calllib(obj.libName,'simxGetAndClearStringSignal',clientID,signalName_,signalValue_,signalLength ,operationMode_);

    if (rtn==0)
        signalValue_.setdatatype('int8Ptr',1,double(signalLength));
        signalValue = char(signalValue_.value);
    else
        signalValue = [];
    end
end