function rtn = simxAddStatusbarMessage(obj,clientID,message,operationmode)
        message_ = libpointer('int8Ptr',[int8(message) 0]);
        operationmode_ = int32(operationmode);

        [rtn message_] = calllib(obj.libName,'simxAddStatusbarMessage',clientID,message_,operationmode_);
end	