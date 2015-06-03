function rtn = simxStopSimulation(obj,clientID,operationMode)
    operationMode_ = int32(operationMode);

    rtn = calllib(obj.libName,'simxStopSimulation',clientID,operationMode_);
end