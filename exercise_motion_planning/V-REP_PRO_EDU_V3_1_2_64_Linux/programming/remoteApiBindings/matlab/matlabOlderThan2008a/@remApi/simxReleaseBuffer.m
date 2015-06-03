function [] = simxReleaseBuffer(obj,buffer)
    buffer_ = calllib(obj.libName,'simxReleaseBuffer',buffer);
end