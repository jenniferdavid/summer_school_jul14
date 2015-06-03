function [rtn prop] = simxGetUIButtonProperty(obj,clientID,uiHandle,uiButtonID,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiButtonID_ = int32(uiButtonID);
    prop = libpointer('int32Ptr',int32(0));

    [rtn prop ] = calllib(obj.libName,'simxGetUIButtonProperty',clientID,uiHandle_,uiButtonID_,prop,operationMode_);
end