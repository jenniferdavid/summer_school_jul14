function [rtn baseHandle]= simxLoadModel(obj,clientID,modelPathAndName,options,operationMode)
    modelPathAndName_ = libpointer('int8Ptr',[int8(modelPathAndName) 0]);
    options_ = int8(options);
    baseHandle= libpointer('int32Ptr',int32(0));
    operationMode_ = int32(operationMode);

    [rtn modelPathAndName_ baseHandle] = calllib(obj.libName,'simxLoadModel',clientID,modelPathAndName_,options_,baseHandle,operationMode_);
end