function rtn = simxBreakForceSensor(obj,clientID,forceSensorHandle,operationMode)
        forceSensorHandle_ = int32(forceSensorHandle);
        operationMode_ = int32(operationMode);

        rtn = calllib(obj.libName, 'simxBreakForceSensor',clientID,forceSensorHandle_,operationMode_);
end    