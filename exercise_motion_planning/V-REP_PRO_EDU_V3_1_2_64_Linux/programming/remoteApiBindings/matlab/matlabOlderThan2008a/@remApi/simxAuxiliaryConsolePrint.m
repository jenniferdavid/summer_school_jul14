function rtn = simxAuxiliaryConsolePrint(obj,clientID,console,text,operationmode)
        console_ = int32(console);
        operationmode_ = int32(operationmode);

        if text
            text_ = libpointer('int8Ptr',[int8(text) 0]);
        else
            text_ = [];
        end

        rtn = calllib(obj.libName,'simxAuxiliaryConsolePrint',clientID,console_,text_,operationmode_);
end  