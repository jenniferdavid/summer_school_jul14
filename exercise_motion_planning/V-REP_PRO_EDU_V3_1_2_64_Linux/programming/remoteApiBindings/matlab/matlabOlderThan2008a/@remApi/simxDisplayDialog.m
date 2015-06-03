function [rtn dialogHandle uiHandle] = simxDisplayDialog(obj,clientID,titleText,mainText,dialogType,initialText,titleColors,dialogColors,operationMode)
        if (numel(titleColors) < 6)&&(numel(titleColors) ~= 0)
            error('titleColors should have 6 values');
            return;
        end
        if (numel(dialogColors) < 6)&&(numel(dialogColors) ~= 0)
            error('dialogColors should have 6 values');
            return;
        end

        titleText_ = libpointer('int8Ptr',[int8(titleText) 0]);
        mainText_ = libpointer('int8Ptr',[int8(mainText) 0]);
        dialogType_ = int32(dialogType);
        initialText_ = libpointer('int8Ptr',[int8(initialText) 0]);
        titleColors_ = libpointer('singlePtr', single(titleColors));
        dialogColors_ = libpointer('singlePtr', single(dialogColors));
        operationMode_ = int32(operationMode);
        dialogHandle = libpointer('int32Ptr',int32(0));
        uiHandle = libpointer('int32Ptr',int32(0));

        [rtn a b c d e dialogHandle uiHandle] = calllib(obj.libName,'simxDisplayDialog',clientID,titleText_,mainText_,dialogType_,initialText_,titleColors_,dialogColors_,dialogHandle,uiHandle,operationMode_);
end 