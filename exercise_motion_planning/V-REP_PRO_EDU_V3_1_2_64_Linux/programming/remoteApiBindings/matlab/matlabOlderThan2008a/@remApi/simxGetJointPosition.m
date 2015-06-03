function [rtn position] = simxGetJointPosition(obj,clientID,handle,option)
        handle_ = int32(handle);
        option_ = int32(option);

        [rtn position] = calllib(obj.libName,'simxGetJointPosition',clientID,handle_,single(43),option_);
end 