function [rtn resolution_ image] = simxGetVisionSensorImage(obj,clientID,handle,options,operationmode)
    resolution = [0 0];
    resolution_ = libpointer('int32Ptr',int32(resolution));
    image = libpointer('int8Ptr',[]);
    options_ = int8(options);
    operationmode_ = int32(operationmode);
    handle_ = int32(handle);

    [rtn resolution_ image] = calllib(obj.libName,'simxGetVisionSensorImage',clientID,handle_,resolution_,image,options_,operationmode_);
end