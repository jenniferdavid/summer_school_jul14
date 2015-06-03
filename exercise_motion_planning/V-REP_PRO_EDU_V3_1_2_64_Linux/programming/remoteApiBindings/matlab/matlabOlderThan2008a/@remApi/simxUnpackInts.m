function [intArray]= simxUnpackInts(obj,string)
    intArray=typecast(uint8(int32(string)),'int32');
end