function [rtn resolution_ image] = simxGetVisionSensorImage2(obj,clientID,handle,options,operationmode)
    resolution = [0 0];
    resolution_ = libpointer('int32Ptr',int32(resolution));
    image_ = libpointer('int8Ptr',[]);
    options_ = int8(options);
    operationmode_ = int32(operationmode);
    handle_ = int32(handle);

    [rtn resolution_ image_] = calllib(obj.libName,'simxGetVisionSensorImage',clientID,handle_,resolution_,image_,options_,operationmode_);

    if (rtn==0)
        if((resolution_(1) ~= 0) && (resolution_(2) ~= 0))
            if(options == 1) %grayscale image
                image_.setdatatype('uint8Ptr',1,resolution_(1)*resolution_(2));
                image = zeros(resolution_(1),resolution_(2));
                image = cast(image,'uint8');
                for i = resolution_(1):-1:1;
                    count = (resolution_(1)-i)*resolution_(2);
                    for j = 1:resolution_(2);
                        image(i,j) = uint8(image_.value(count+j));
                    end
                end
            else 
                image_.setdatatype('uint8Ptr',1,resolution_(1)*resolution_(2)*3);
                image = zeros(resolution_(1),resolution_(2),3);
                image = cast(image,'uint8');
                for i = resolution_(1):-1:1;
                    count = (resolution_(1)-i)*resolution_(2)*3;
                    for j = 1:3:resolution_(2)*3;
                        for k=1:3;
                            l=int32(j/3) +1;
                            image(i,l,k) = uint8(image_.value(count+j+k-1));  
                        end
                    end
                end
            end
        end
    else
        image = [];
    end
end