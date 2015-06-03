function buffer = simxCreateBuffer(obj,bufferSize)
    bufferSize_ = int32(bufferSize);

    buffer = calllib(obj.libName,'simxCreateBuffer',bufferSize_);
end