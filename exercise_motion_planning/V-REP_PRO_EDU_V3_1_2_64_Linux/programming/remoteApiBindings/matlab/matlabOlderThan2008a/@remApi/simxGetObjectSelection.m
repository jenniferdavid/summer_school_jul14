function [rtn objectHandles ] = simxGetObjectSelection(obj,clientID,operationMode)
    objectHandles_ = libpointer('int32PtrPtr');
    objectCount = libpointer('int32Ptr',int32(0));
    operationMode_ = int32(operationMode);

    [rtn objectHandles_ objectCount] = calllib(obj.libName,'simxGetObjectSelection',clientID,objectHandles_ ,objectCount,operationMode_);

    if (rtn==0)	
        if(objectCount > 0)
            objectHandles_.setdatatype('int32Ptr',1,objectCount);
            objectHandles = objectHandles_.value;
        else
            objectHandles = [];
        end
    else
        objectHandles = [];
    end
end