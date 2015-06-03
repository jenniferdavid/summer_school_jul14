function rtn = simxSynchronous(obj,clientID,enable)
    enable_ = int8(enable);

    rtn = calllib(obj.libName,'simxSynchronous',clientID,enable_);
end