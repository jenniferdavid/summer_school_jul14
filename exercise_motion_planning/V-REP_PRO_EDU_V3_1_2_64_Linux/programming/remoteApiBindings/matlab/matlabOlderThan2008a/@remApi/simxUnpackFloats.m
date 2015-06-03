function [floatArray]= simxUnpackFloats(obj,string)
    floatArray=typecast(uint8(single(string)),'single');
end