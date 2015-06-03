function rtn = simxSynchronousTrigger(obj,clientID)
    rtn = calllib(obj.libName,'simxSynchronousTrigger',clientID);
end