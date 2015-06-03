function [rtn childObjectHandle] = simxGetObjectChild(obj,clientID,parentObjectHandle,childIndex,operationMode)
    parentObjectHandle_ = int32(parentObjectHandle);
    childIndex_ = int32(childIndex);
    childObjectHandle = libpointer('int32Ptr',int32(0));    
    operationMode_ = int32(operationMode);

    [rtn childObjectHandle ] = calllib(obj.libName,'simxGetObjectChild',clientID,parentObjectHandle_,childIndex_,childObjectHandle,operationMode_);
end