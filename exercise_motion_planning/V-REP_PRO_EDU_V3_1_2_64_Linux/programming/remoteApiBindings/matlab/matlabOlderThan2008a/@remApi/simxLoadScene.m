function [rtn ]= simxLoadScene(obj,clientID,scenePathAndName,options,operationMode)
    scenePathAndName_ = libpointer('int8Ptr',[int8(scenePathAndName) 0]);
    options_ = int8(options);
    operationMode_ = int32(operationMode);

    [rtn scenePathAndName_ ] = calllib(obj.libName,'simxLoadScene',clientID,scenePathAndName_,options_,operationMode_);
end