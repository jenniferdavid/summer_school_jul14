function [rtn info]= simxGetOutMessageInfo(obj,clientID,infoType)
    infoType_ = int32(infoType);
    info = libpointer('int32Ptr',int32(0));

    [rtn info] = calllib(obj.libName,'simxGetOutMessageInfo',clientID,infoType_,info);
end