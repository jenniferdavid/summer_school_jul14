function [rtn ]= simxSetObjectOrientation(obj,clientID,objectHandle,relativeToObjectHandle,eulerAngles,operationMode)
    objectHandle_ = int32(objectHandle);
    relativeToObjectHandle_ = int32(relativeToObjectHandle);
    num_ele = numel(eulerAngles);
    if (num_ele < 3)
        error('Euler angles should have 3 values');
        return;
    else
    eulerAngles_ = libpointer('singlePtr',single(eulerAngles));
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetObjectOrientation',clientID,objectHandle_,relativeToObjectHandle_,eulerAngles_,operationMode_);
    end
end