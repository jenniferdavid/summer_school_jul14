function [rtn force]= simxJointGetForce(obj,clientID,jointHandle,operationMode)
    jointHandle_ = int32(jointHandle);
    operationMode_ = int32(operationMode);
    force = libpointer('single',single(0));

    [rtn force ] = calllib(obj.libName,'simxJointGetForce',clientID,jointHandle_,single(0),operationMode_);
end