function [rtn handle] = simxGetUIHandle(obj,clientID,uiName,operationMode)
    uiName_ = libpointer('int8Ptr',[int8(uiName) 0]);
    operationMode_ = int32(operationMode);
    handle = libpointer('int32Ptr',int32(0));

    [rtn uiName handle] = calllib(obj.libName,'simxGetUIHandle',clientID,uiName_,handle,operationMode_);
end