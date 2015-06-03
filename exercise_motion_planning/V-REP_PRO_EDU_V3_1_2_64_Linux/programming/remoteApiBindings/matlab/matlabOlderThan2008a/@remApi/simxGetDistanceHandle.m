function [rtn handle] = simxGetDistanceHandle(obj,clientID,distanceObjectName,operationMode)
        distanceObjectName_ = libpointer('int8Ptr',[int8(distanceObjectName) 0]);
        handle_ = libpointer('int32Ptr',int32(0));
        operationMode_ = int32(operationMode);

        [rtn distanceObjectName_ handle] = calllib(obj.libName,'simxGetDistanceHandle',clientID,distanceObjectName_,handle_ ,operationMode_);
end 