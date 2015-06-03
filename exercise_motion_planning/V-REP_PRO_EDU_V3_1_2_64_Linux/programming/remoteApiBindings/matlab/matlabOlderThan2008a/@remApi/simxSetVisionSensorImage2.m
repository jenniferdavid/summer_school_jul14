function rtn = simxSetVisionSensorImage2(obj,clientID,handle,image,operationmode)
    handle_ = int32(handle);
    [m n o] = size(image);
    buffsize_ = int32(m*n*o);
    if(o == 1)
        options_ = int8(1);
    else
        options_ = int8(0);
    end
    operationmode_ = int32(operationmode);
    imdata = zeros(1,buffsize_);
    imdata= cast(imdata,'uint8');

    for i = m:-1:1;
        count = (m-i)*n*o;
        for j = 1:o:n*o;
            for k=1:o;
                if(o==1)
                    l=j;
                else
                    l=int32(j/o) +1;
                end
                imdata(count+j+k-1) = image(i,l,k);
            end
        end
    end

    image_ = libpointer('int8Ptr',imdata);

    [rtn image_] = calllib(obj.libName,'simxSetVisionSensorImage',clientID,handle_,image_,buffsize_,options_,operationmode_); 
end