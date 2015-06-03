function rtn = simxGetConnectionId(obj,clientID)
        rtn = calllib(obj.libName,'simxGetConnectionId',clientID);
end 