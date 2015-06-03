clear all
close all 
clc

%Please run Internal Example.ttt first
%The plugin allows internal and external reading simultaneously.
%Reading data is almost snapshot, so synchronization with the simulator 
%is simpler.
%It isn't advisable to write memory from two places simultaneously.
%It is preferable to create 2 memories, a read and a write
i=0
while(i<1000)
    i=i+1;
    [ret,data]=simExtMatlabSM('helloWord',int32(20));
    disp(char(data));
    pause(0.025)
    clc
end
