function [rtn resolution_ buffer] = simxGetVisionSensorDepthBuffer(obj,clientID,handle,operationmode)
    resolution = [0 0];
    resolution_ = libpointer('int32Ptr',int32(resolution));
    buffer = libpointer('singlePtr',[]);
    operationmode_ = int32(operationmode);
    handle_ = int32(handle);

    [rtn resolution_ buffer] = calllib(obj.libName,'simxGetVisionSensorDepthBuffer',clientID,handle_,resolution_,buffer,operationmode_);
end