function rtn = simxPauseCommunication(obj,clientID,enable)
    enable_ = int8(enable);

    rtn = calllib(obj.libName,'simxPauseCommunication',clientID,enable_);
end