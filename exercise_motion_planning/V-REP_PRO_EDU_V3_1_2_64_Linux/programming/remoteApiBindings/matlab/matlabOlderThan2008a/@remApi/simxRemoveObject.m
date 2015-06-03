function [rtn ] = simxRemoveObject(obj,clientID,objectHandle,operationMode)
        objectHandle_ = int32(objectHandle);
        operationMode_ = int32(operationMode);

        [rtn ]  = calllib(obj.libName,'simxRemoveObject',clientID,objectHandle_,operationMode_);
end	