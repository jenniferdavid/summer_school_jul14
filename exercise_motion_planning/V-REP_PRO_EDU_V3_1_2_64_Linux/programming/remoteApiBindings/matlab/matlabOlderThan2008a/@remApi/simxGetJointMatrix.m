function [rtn matrix] = simxGetJointMatrix(obj,clientID,jointHandle,operationMode)
        jointHandle_ = int32(jointHandle);
        matrix = libpointer('singlePtr',single([0 0 0 0 0 0 0 0 0 0 0 0]));
        operationMode_ = int32(operationMode);

       [rtn matrix ] = calllib(obj.libName,'simxGetJointMatrix',clientID,jointHandle_,matrix, operationMode_);
end 