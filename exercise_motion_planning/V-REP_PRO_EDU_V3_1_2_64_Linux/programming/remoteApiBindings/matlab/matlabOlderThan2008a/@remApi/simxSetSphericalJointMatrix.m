function [rtn ]= simxSetSphericalJointMatrix(obj,clientID,jointHandle,matrix,operationMode)
    jointHandle_ = int32(jointHandle);
    num_ele = numel(matrix);
    if (num_ele < 12)
        error('matrix should have 12 values');
        return;
    else
        matrix_ = libpointer('singlePtr',single(matrix));
        operationMode_ = int32(operationMode);

        [rtn signalName_ ] = calllib(obj.libName,'simxSetSphericalJointMatrix',clientID,jointHandle_,matrix_,operationMode_);
    end
end