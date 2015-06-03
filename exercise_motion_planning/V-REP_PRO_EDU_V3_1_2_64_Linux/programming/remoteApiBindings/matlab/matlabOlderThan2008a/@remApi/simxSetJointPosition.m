function rtn = simxSetJointPosition(obj,clientID,handle,position,option)
    handle_ = int32(handle);
    position_ = single(position);
    option_ = int32(option);
    rtn = calllib(obj.libName,'simxSetJointPosition',clientID,handle_,position_,option_);
end