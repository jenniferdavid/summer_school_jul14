function [rtn handle] = simxGetObjectHandle(obj,clientID,name,operationmode)
    name_ptr = libpointer('int8Ptr',[int8(name) 0]);
    handle_ptr = libpointer('int32Ptr',int32(0));
    operationmode_ = int32(operationmode);

    [rtn name_ptr handle] = calllib(obj.libName,'simxGetObjectHandle',clientID,name_ptr,handle_ptr,operationmode_);
end