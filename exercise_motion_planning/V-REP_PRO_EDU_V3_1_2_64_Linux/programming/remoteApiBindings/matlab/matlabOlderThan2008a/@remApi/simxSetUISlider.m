function [rtn] = simxSetUISlider (obj,clientID,uiHandle,uiButtonID,position,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiButtonID_ = int32(uiButtonID);
    position_ = int32(position);

    [rtn] = calllib(obj.libName,'simxSetUISlider',clientID,uiHandle_,uiButtonID_,position_,operationMode_)
end