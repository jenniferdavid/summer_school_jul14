function [rtn detectionState detectedPoint detectedObjectHandle detectedSurfaceNormalVector]= simxReadProximitySensor(obj,clientID,sensorHandle,operationMode)
        sensorHandle_ = int32(sensorHandle);
        detectionState = libpointer('int8Ptr', int8(0));
        detectedPoint = libpointer('singlePtr', single([0 0 0]));
        detectedSurfaceNormalVector = libpointer('singlePtr', single([0 0 0]));
        detectedObjectHandle = libpointer('int32Ptr',int32(0));
        operationMode_ = int32(operationMode);

        [rtn detectionState detectedPoint detectedObjectHandle detectedSurfaceNormalVector] = calllib(obj.libName,'simxReadProximitySensor',clientID,sensorHandle_,detectionState ,detectedPoint , detectedObjectHandle ,detectedSurfaceNormalVector,operationMode_);
end	