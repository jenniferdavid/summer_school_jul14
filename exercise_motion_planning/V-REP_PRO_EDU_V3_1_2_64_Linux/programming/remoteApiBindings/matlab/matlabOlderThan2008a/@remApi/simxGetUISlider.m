function [rtn position] = simxGetUISlider(obj,clientID,uiHandle,uiButtonID,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiButtonID_ = int32(uiButtonID);
    position = libpointer('int32Ptr',int32(0));

    [rtn position ] = calllib(obj.libName,'simxGetUISlider',clientID,uiHandle_,uiButtonID_,position,operationMode_)
end