function [rtn retHandles retInts retFloats retStrings]= simxGetObjectGroupData(obj,clientID,objectType,dataType,operationMode)
    objectType_ = int32(objectType);
    dataType_ = int32(dataType);
    operationMode_ = int32(operationMode);

    handlesCount_ = libpointer('int32Ptr',int32(0));
    intsCount_ = libpointer('int32Ptr',int32(0));
    floatsCount_ = libpointer('int32Ptr',int32(0));
    stringsCount_ = libpointer('int32Ptr',int32(0));

    retHandles_ = libpointer('int32PtrPtr');
    retInts_ = libpointer('int32PtrPtr');
    retFloats_ = libpointer('singlePtrPtr');
    retStrings_ = libpointer('int8PtrPtr');

    [rtn handlesCount_ retHandles_ intsCount_ retInts_ floatsCount_ retFloats_ stringsCount_ retStrings_ ] = calllib(obj.libName,'simxGetObjectGroupData',clientID,objectType_,dataType_,handlesCount_,retHandles_,intsCount_,retInts_,floatsCount_,retFloats_,stringsCount_,retStrings_,operationMode_);

    if (rtn==0) 
        if (handlesCount_~=0)
            retHandles_.setdatatype('int32Ptr',1,handlesCount_);
            retHandles = retHandles_.value;
        else
            retHandles=[];
        end

        if (intsCount_~=0)
            retInts_.setdatatype('int32Ptr',1,intsCount_);
            retInts = retInts_.value;
        else
            retInts=[];
        end

        if (floatsCount_~=0)
            retFloats_.setdatatype('singlePtr',1,floatsCount_);
            retFloats = retFloats_.value;
        else
            retFloats=[];
        end

        retStrings = cell(double(stringsCount_));
        s=1;
        for i=1:stringsCount_
            begin = s;
            retStrings_.setdatatype('int8Ptr',1,s);
            value = retStrings_.value(s);
            while(value ~= 0)
                retStrings_.setdatatype('int8Ptr',1,s);
                value = retStrings_.value(s);
                s=s+1;
            end
            tmp = retStrings_.value(begin:s-1);
            retStrings(i) = cellstr(char(tmp));
        end

    else
        retHandles=[];
        retInts=[];
        retFloats=[];
        retStrings=[];
    end
end