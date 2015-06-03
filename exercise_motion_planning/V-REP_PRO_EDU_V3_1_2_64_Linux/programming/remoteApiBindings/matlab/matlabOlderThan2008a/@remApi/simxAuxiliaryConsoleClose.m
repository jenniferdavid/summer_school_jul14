function rtn = simxAuxiliaryConsoleClose(obj,clientID,console, operationmode)
    console_ = int32(console);
    operationmode_ = int32(operationmode);

    rtn = calllib(obj.libName,'simxAuxiliaryConsoleClose',clientID,console_,operationmode_)
end