function [rtn detectionState auxValues auxValuesCount ] = simxReadVisionSensor(obj,clientID,sensorHandle,operationmode)
        detectionState = libpointer('int8Ptr',int8(0));
        auxValues_ = libpointer('singlePtrPtr');
        auxValuesCount_ = libpointer('int32PtrPtr');
        operationmode_ = int32(operationmode);
        sensorHandle_ = int32(sensorHandle);

        [rtn detectionState auxValues_ auxValuesCount_] = calllib(obj.libName,'simxReadVisionSensor',clientID,sensorHandle_,detectionState ,auxValues_,auxValuesCount_,operationmode_);

        auxValues = [];
        auxValuesCount = [];
        if(rtn == 0)
            auxValuesCount_.setdatatype('int32Ptr',1,999);
            packets=auxValuesCount_.value(1);
            auxValues_.setdatatype('singlePtr',single(999));
            n=1;
            for i=1:packets
                cnt=auxValuesCount_.value(i+1);
                auxValuesCount=[auxValuesCount cnt];
                for j=1:cnt
                    v=auxValues_.value(n);
                    n=n+1;
                    auxValues=[auxValues v];
                end
            end
            auxValuesCount_.setdatatype('int8Ptr',2,2);
            auxValues_.setdatatype('int8Ptr',2,2);
            obj.simxReleaseBuffer(auxValuesCount_);
            obj.simxReleaseBuffer(auxValues_);
        end
end	