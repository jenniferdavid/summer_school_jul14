function [rtn uiHandles] = simxLoadUI(obj,clientID,uiPathAndName,options,operationMode)
    uiPathAndName_ = libpointer('int8Ptr',int8([uiPathAndName 0]));
    options_ = int8(options);
    count = libpointer('int32Ptr', int32(0));
    uiHandles_ = libpointer('int32Ptr');
    operationMode_ = int32(operationMode);

    [rtn uiPathAndName_ count uiHandles_ ] = calllib(obj.libName,'simxLoadUI',clientID,uiPathAndName_,options_,count ,uiHandles_ ,operationMode_);

    if (rtn==0)
        if(count > 0)
            uiHandles_.setdatatype('int32Ptr',1,count);
            uiHandles = uiHandles_.value;
        else
            uiHandles = [];
        end
        uiHandles_.setdatatype('int8Ptr',2,2);
        obj.simxReleaseBuffer(uiHandles_);
    else
        uiHandles = [];
    end
end