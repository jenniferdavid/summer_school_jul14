function [rtn] = simxStart(obj,server,port,waitUntilConnected,doNotReconnectOnceDisconnected,timeOutInMs,commThreadCycleInMs)
    server_intval = int8([server,0]);
    server_ptr = libpointer('int8Ptr',server_intval);	    
    rtn = calllib(obj.libName,'simxStart',server_ptr,port,int8(waitUntilConnected),int8(doNotReconnectOnceDisconnected),timeOutInMs,commThreadCycleInMs);
end
