function [string]= simxPackInts(obj,intArray)
    string=char(typecast(int32(intArray),'uint8'));
end