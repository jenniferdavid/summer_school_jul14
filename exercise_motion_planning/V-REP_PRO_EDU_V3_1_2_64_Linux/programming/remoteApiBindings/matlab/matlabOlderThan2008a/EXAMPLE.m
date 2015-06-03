%% VRep API with old object oriented programming framework
% Example implementation of the VRep Remote API for 32bit matlab version older
% than 2008a (a new object oriented programming style was introduced using
% 'classdef' which is not supported in older versions)
% Information on that:
%  https://yagtom.googlecode.com/svn-history/trunk/html/objectOriented.html


% start remoteApi
	disp('Program started');
	vrep=remApi('remoteApi','extApi.h');
	clientID=simxStart(vrep,'127.0.0.1',19999,true,true,5000,5);
 	if (clientID>-1)
		disp(['Connected to remote API server, clientID ', int2str(clientID)]);
        loadRemApiConstants; % load constants of vrep
        simxStartSimulation(vrep,clientID,simx_opmode_oneshot_wait);
% EXAMPLE CODE
            [errorCode,quadcopterhandle]=simxGetObjectHandle(vrep,clientID,'Quadricopter',simx_opmode_oneshot_wait);     
            simxSetObjectPosition(vrep,clientID,quadcopterhandle,-1,[0 0 0],simx_opmode_oneshot);
            simxSetObjectOrientation(vrep,clientID,quadcopterhandle,-1,[1 1 1],simx_opmode_oneshot);
            disp('position1');
            pause(1);
            simxSetObjectPosition(vrep,clientID,quadcopterhandle,-1,[1 1 1],simx_opmode_oneshot);
            simxSetObjectOrientation(vrep,clientID,quadcopterhandle,-1,[0 0 0],simx_opmode_oneshot);
            disp('position2');
% stop remoteApi
        simxStopSimulation(vrep,clientID,simx_opmode_oneshot_wait);

        unloadRemApiConstants;
        simxFinish(vrep,clientID);
    delete(vrep); % explicitely call the destructor!
	disp('Program ended');
 end
