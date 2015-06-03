function [rtn] = simxSetObjectSelection(obj,clientID,objectHandles,operationMode)
    objectHandles_ = libpointer('int32Ptr',int32(objectHandles));
    objectCount = numel(objectHandles);
    operationMode_ = int32(operationMode);

    [rtn objectHandles_ ] = calllib(obj.libName,'simxSetObjectSelection',clientID,objectHandles_ ,objectCount,operationMode_);
end