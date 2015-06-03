function [rtn uiEventButtonID auxValues] = simxGetUIEventButton(obj,clientID,uiHandle,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiEventButtonID = libpointer('int32Ptr',int32(0));
    auxValues = libpointer('int32Ptr',int32([0 0]));

    [rtn uiEventButtonID auxValues] = calllib(obj.libName,'simxGetUIEventButton',clientID,uiHandle_,uiEventButtonID ,auxValues ,operationMode_);
end