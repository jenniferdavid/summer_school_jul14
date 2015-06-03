function rtn = simxSetVisionSensorImage(obj,clientID,handle,image,buffsize,options,operationmode)
    handle_ = int32(handle);
    buffsize_ = int32(buffsize);
    options_ = int8(options);
    operationmode_ = int32(operationmode);

    [rtn image] = calllib(obj.libName,'simxSetVisionSensorImage',clientID,handle_,image,buffsize_,options_,operationmode_);
end