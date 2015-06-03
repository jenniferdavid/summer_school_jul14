function rtn = simxStartSimulation(obj,clientID,operationMode)
    operationMode_ = int32(operationMode);

    rtn = calllib(obj.libName,'simxStartSimulation',clientID,operationMode_);
end