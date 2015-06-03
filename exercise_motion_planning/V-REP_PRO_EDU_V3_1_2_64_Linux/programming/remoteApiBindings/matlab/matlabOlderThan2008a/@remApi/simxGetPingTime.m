function [rtn pingTime]= simxGetPingTime(obj,clientID)
    pingTime = libpointer('int32Ptr',int32(0));

    [rtn pingTime] = calllib(obj.libName,'simxGetPingTime',clientID,pingTime);
end