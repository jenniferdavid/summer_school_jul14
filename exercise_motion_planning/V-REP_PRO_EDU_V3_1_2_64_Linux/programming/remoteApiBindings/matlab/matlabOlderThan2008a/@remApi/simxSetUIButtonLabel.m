function [rtn ] = simxSetUIButtonLabel(obj,clientID,uiHandle,uiButtonID,upStateLabel,downStateLabel,operationMode)
    uiHandle_ = int32(uiHandle);
    operationMode_ = int32(operationMode);
    uiButtonID_ = int32(uiButtonID);
    upStateLabel_ = libpointer('int8Ptr',int8([upStateLabel 0]));
    downStateLabel_ = libpointer('int8Ptr',int8([downStateLabel 0]));

    [rtn upStateLabel_ downStateLabel_] = calllib(obj.libName,'simxSetUIButtonLabel',clientID,uiHandle_,uiButtonID_,upStateLabel_,downStateLabel_,operationMode_);
end