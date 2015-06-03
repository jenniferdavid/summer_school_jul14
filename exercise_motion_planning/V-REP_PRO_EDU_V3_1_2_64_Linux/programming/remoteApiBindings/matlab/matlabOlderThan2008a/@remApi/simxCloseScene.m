function rtn = simxCloseScene(obj,clientID,operationMode)
    operationMode_ = int32(operationMode);

    rtn = calllib(obj.libName,'simxCloseScene',clientID,operationMode_);
end