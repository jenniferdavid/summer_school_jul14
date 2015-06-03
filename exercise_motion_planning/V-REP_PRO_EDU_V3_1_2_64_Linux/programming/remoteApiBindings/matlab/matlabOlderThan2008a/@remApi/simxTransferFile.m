function rtn = simxTransferFile(obj,clientID,filePathAndName,fileName_serverSide,timeOut,operationMode)
    filePathAndName_ = libpointer('int8Ptr',[int8(filePathAndName) 0]);
    fileName_serverSide_ = libpointer('int8Ptr',[int8(fileName_serverSide) 0]);
    timeOut_ = int32(timeOut);
    operationMode_ = int32(operationMode);

    [rtn filePathAndName_ fileName_serverSide_] = calllib(obj.libName,'simxTransferFile',clientID,filePathAndName_,fileName_serverSide_,timeOut_,operationMode_);
end