function [rtn ] = simxSetUIButtonProperty(obj,clientID,uiHandle,uiButtonID,prop,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiButtonID_ = int32(uiButtonID);
    prop_ = int32(prop);

    [rtn ] = calllib(obj.libName,'simxSetUIButtonProperty',clientID,uiHandle_,uiButtonID_,prop_,operationMode_);
end