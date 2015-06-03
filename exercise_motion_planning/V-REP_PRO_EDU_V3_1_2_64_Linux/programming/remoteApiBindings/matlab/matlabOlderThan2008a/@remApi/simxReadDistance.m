function [rtn minimumDistance]= simxReadDistance(obj,clientID,distanceObjectHandle,operationMode)
    distanceObjectHandle_ = int32(distanceObjectHandle);
    operationMode_ = int32(operationMode);
    minimumDistance = libpointer('singlePtr',single(0));

    [rtn minimumDistance] = calllib(obj.libName,'simxReadDistance',clientID,distanceObjectHandle_,minimumDistance,operationMode_);
end