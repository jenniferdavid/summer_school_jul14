function [rtn handle] = simxGetCollisionHandle(obj,clientID,collisionObjectName,operationMode)
        collisionObjectName_ = libpointer('int8Ptr',[int8(collisionObjectName) 0]);
        handle_ = libpointer('int32Ptr',int32(0));
        operationMode_ = int32(operationMode);

        [rtn collisionObjectName_ handle] = calllib(obj.libName,'simxGetCollisionHandle',clientID,collisionObjectName_,handle_ ,operationMode_);
end 