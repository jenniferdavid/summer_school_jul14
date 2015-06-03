function [rtn resolution_ buffer] = simxGetVisionSensorDepthBuffer2(obj,clientID,handle,operationmode)
    resolution = [0 0];
    resolution_ = libpointer('int32Ptr',int32(resolution));
    buffer_ = libpointer('singlePtr',[]);
    operationmode_ = int32(operationmode);
    handle_ = int32(handle);

    [rtn resolution_ buffer_] = calllib(obj.libName,'simxGetVisionSensorDepthBuffer',clientID,handle_,resolution_,buffer_,operationmode_);

    if (rtn==0)
        if((resolution_(1) ~= 0) && (resolution_(2) ~= 0))
            buffer_.setdatatype('singlePtr',1,resolution_(1)*resolution_(2));
            buffer = zeros(resolution_(1),resolution_(2));
            buffer = cast(buffer,'single');
            for i = resolution_(1):-1:1;
                count = (resolution_(1)-i)*resolution_(2);
                for j = 1:resolution_(2);
                    buffer(i,j) = single(buffer_.value(count+j));
                end
            end
        end
    else
        buffer = [];
    end
end