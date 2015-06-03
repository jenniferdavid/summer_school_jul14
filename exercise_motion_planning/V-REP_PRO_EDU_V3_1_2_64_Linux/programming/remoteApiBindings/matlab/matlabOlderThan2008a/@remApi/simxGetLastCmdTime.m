function rtn = simxGetLastCmdTime(obj,clientID)
    rtn = calllib(obj.libName,'simxGetLastCmdTime',clientID);
end