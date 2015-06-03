function [rtn newObjectHandles] = simxCopyPasteObjects(obj,clientID,objectHandles,operationMode)
    objectHandles_ = libpointer('int32Ptr',int32(objectHandles));
    objectCount_ = int32(numel(objectHandles));
    newObjectHandles_ = libpointer('int32Ptr',[]);
    newObjectCount_ = libpointer('int32Ptr',int32(0));
    operationMode_ = int32(operationMode);

    [rtn objectHandles_ newObjectHandles_ newObjectCount_] = calllib(obj.libName,'simxCopyPasteObjects',clientID,objectHandles_,objectCount_,newObjectHandles_,newObjectCount_,operationMode_);

    if (rtn==0)&&(newObjectCount_>0) 
        newObjectHandles = zeros(1,newObjectCount_);
        newObjectHandles_.setdatatype('int32Ptr',1,newObjectCount_);
        for i=1:newObjectCount_;
            newObjectHandles(i) = newObjectHandles_.value(i);
        end
    else
        newObjectHandles=[];
    end
end