function [string]= simxPackFloats(obj,floatArray)
    string=char(typecast(single(floatArray),'uint8'));
end