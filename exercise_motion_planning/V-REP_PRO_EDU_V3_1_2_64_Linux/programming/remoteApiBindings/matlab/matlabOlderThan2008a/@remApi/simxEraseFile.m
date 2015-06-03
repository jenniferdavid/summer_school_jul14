function rtn = simxEraseFile(obj,clientID,fileName_serverSide,operationMode)
        fileName_serverSide_ = libpointer('int8Ptr',[int8(fileName_serverSide) 0]);
        operationMode_ = int32(operationMode);

        [rtn fileName_serverSide_] = calllib(obj.libName,'simxEraseFile',clientID,fileName_serverSide_,operationMode_);
end 