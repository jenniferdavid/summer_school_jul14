function rtn = simxSetObjectPosition(obj,clientID,handle,rel_pos,position,option)
    if (numel(position) < 3)
        error('position should have 3 values');
        return;
    end

    handle_ = int32(handle);
    rel_pos_ = int32(rel_pos);
    option_ = int32(option);
    pos_ptr = libpointer('singlePtr',single(position));

   [rtn pos_ptr] = calllib(obj.libName,'simxSetObjectPosition',clientID,handle_,rel_pos_,pos_ptr,option_);
end