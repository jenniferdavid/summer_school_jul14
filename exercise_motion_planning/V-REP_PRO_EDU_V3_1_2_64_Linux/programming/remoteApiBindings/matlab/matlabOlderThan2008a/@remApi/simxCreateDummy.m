function [rtn handle] = simxCreateDummy(obj,clientID,size,colors,operationmode)
    size_ = single(size);
    mode_ = int32(mode);

    if (numel(colors) < 12)&&(numel(colors) ~= 0)
        error('colors should have 12 values');
        return;
    end

    color_ = libpointer('int8Ptr',int8(colors));
    handle_ = libpointer('int32Ptr',int32(0));

    [rtn handle] = calllib(obj.libName,'simxCreateDummy',clientID,size_,color_,handle_,operationmode);
end