function [rtn console_handle] = simxAuxiliaryConsoleOpen(obj,clientID,title,maxLines,mode,position,size,textcolor,backgroundcolor,operationmode)
    title_ = libpointer('int8Ptr',[int8(title) 0]);
    maxLines_ = int32(maxLines);
    mode_ = int32(mode);

    if (numel(position) < 2)&&(numel(position) ~= 0)
        error('position should have 2 values');
        return;
    end
    if (numel(size) < 2)&&(numel(size) ~= 0)
        error('size should have 2 values');
        return;
    end
    if (numel(textcolor) < 3)&&(numel(textcolor) ~= 0)
        error('textcolor should have 3 values');
        return;
    end
    if (numel(backgroundcolor) < 3)&&(numel(backgroundcolor) ~= 0)
        error('backgroundcolor should have 3 values');
        return;
    end

    position_ = libpointer('int32Ptr',int32(position));
    size_ = libpointer('int32Ptr',int32(size));
    textcolor_ = libpointer('singlePtr',single(textcolor));
    backgroundcolor_ = libpointer('singlePtr',single(backgroundcolor));
    consolehandle_ = libpointer('int32Ptr',int32(0));
    operationmode_ = int32(operationmode);

    [rtn a b c d e console_handle] = calllib(obj.libName,'simxAuxiliaryConsoleOpen',clientID,title_,maxLines_,mode_,position_,size_,textcolor_,backgroundcolor_,consolehandle_,operationmode);
end