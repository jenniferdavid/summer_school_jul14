function [rtn ]= simxSetJointForce(obj,clientID,objectHandle,targetVelocity,operationMode)
    objectHandle_ = int32(objectHandle);
    targetVelocity_ = single(targetVelocity);
    operationMode_ = int32(operationMode);

    [rtn ] = calllib(obj.libName,'simxSetJointForce',clientID,objectHandle_,targetVelocity_,operationMode_);
end