function [rtn state forceVector torqueVector]= simxReadForceSensor(obj,clientID,forceSensorHandle,operationMode)
        forceSensorHandle_ = int32(forceSensorHandle);
        state = libpointer('int8Ptr', int8(0));
        forceVector = libpointer('singlePtr', single([0 0 0]));
        torqueVector = libpointer('singlePtr', single([0 0 0]));
        operationMode_ = int32(operationMode);

        [rtn state forceVector torqueVector] = calllib(obj.libName,'simxReadForceSensor',clientID,forceSensorHandle_,state ,forceVector, torqueVector,operationMode_);
end	