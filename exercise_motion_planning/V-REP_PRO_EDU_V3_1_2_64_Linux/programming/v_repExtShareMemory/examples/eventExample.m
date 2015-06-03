mex matlabSetEvent.cpp
mex matlabResetEvent.cpp
mex matlabWaitForEvent.cpp

%Vrep unlocks Matlab, Vrep locks. Matlab unlock Vrep them Matlab Locks

i=0;
matlabSetEvent('unlockVREP')
while(i<1000)
    matlabSetEvent('unlockVREP')
    pause(0.025)
    i=i+1;
end
matlabSetEvent('unlockVREP')
