function [rtn collisionState]= simxReadCollision(obj,clientID,collisionObjectHandle,operationMode)
        collisionObjectHandle_ = int32(collisionObjectHandle);
        operationMode_ = int32(operationMode);
        collisionState = libpointer('int8Ptr',int8(0));

        [rtn collisionState] = calllib(obj.libName,'simxReadCollision',clientID,collisionObjectHandle_,collisionState,operationMode_);
end	