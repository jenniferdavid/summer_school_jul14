function [rtn retSignalValue]= simxQuery(obj,clientID,signalName,signalValue,retSignalName,timeOutInMs)
    signalLength_ = int32(length(signalValue));
    signalName_ = libpointer('int8Ptr',[int8(signalName) 0]);
    signalValue_ = libpointer('int8Ptr',[int8(signalValue) 0]);
    retSignalName_ = libpointer('int8Ptr',[int8(retSignalName) 0]);
    retSignalValue_ = libpointer('int8PtrPtr');
    retSignalLength_= libpointer('int32Ptr',int32(0));
    timeOutInMs_ = int32(timeOutInMs);

    [rtn signalName_,signalValue_, retSignalName_, retSignalValue_, retSignalLength_] = calllib(obj.libName,'simxQuery',clientID,signalName_,signalValue_,signalLength_, retSignalName_, retSignalValue_, retSignalLength_ ,timeOutInMs_);

    if (rtn==0)
        retSignalValue_.setdatatype('int8Ptr',1,double(retSignalLength_));
        retSignalValue = char(retSignalValue_.value);
    else
        retSignalValue = [];
    end
end