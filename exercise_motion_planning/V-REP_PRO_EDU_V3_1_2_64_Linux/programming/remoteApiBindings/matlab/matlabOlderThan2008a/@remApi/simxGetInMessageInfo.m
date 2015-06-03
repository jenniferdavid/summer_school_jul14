function [rtn info]= simxGetInMessageInfo(obj,clientID,infoType)
        infoType_ = int32(infoType);
        info = libpointer('int32Ptr',int32(0));

        [rtn info] = calllib(obj.libName,'simxGetInMessageInfo',clientID,infoType_,info);
end 