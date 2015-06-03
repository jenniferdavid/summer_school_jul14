function rtn = simxPauseSimulation(obj,clientID,operationMode)
        operationMode_ = int32(operationMode);

        rtn = calllib(obj.libName,'simxPauseSimulation',clientID,operationMode_);
end	