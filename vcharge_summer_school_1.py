#!/usr/bin/python

import pylab as pl
import numpy as np
import time
import sys
from IPython.core.debugger import Tracer

import vcharge_summer_school_python as vrep; reload(vrep)



# ********************************* #
# Excercise: Fill in this function
# Missing parts are marked with TODO
# ********************************* #

# NOTE: If you are more experienced with MATLAB, you can find python help here
# http://mathesaurus.sourceforge.net/matlab-numpy.html

# TODO: Some parameters for you to play around with
parameters = {'useAngularRateForMotionPredictions': True, # If set to True, the angular velocity will be used for motion predictions of moving objects in the scene
              'plotTrajectorySetInVrep': False, # Plots the trajectory set in V-Rep, but consider setting it to False eventually since it slows down operation quite a lot
              'plotInPython': False } # Plots various things in python, but consider setting it to False eventually since it slows down operation quite a lot


def getBestTrajectory(trajectorySet, pathDistanceMap, lastSolution):
  ''' 
  Extract the best trajectory out of the trajectory set based on a custom cost function. 
  Inputs:
    -trajectorySet: a list of (trajectory, collisionFlag, closestDistanceToStaticObstacles) tuples. "collisionFlag" will be equal to np.inf if the edge is in collision or -1.0 otherwise. That means you only have to care about trajectories with collisionFlag = -1.0 or not np.isinf(collisionFlag) respectively.
    -pathDistanceMap: A grid of distances to the reference path. Use "dist = pathDistanceMap.getByWorldCoordinates(s.state.x, s.state.y)" to compute the distance of a specific state to the reference path
    -lastSolution: Your solution trajectory computed in the previous timestep. You could use this to introduce regularizing cost terms
  '''
  
  #Trajectory has the following attributes:
    #-samples: samples along the trajectory
      #-state: state at this samples
        #-x: x-position [m]
        #-y: y-position [m]
        #-heading: heading [rad]
        #-speed: translational velocity [m/s]
        #-steerAngle: single-track steer angle [rad]
        #-steerSpeed: single-track steer speed [rad/s]
        #-acc: translational acceleration [m/s^2]
        #-time: timestamp of this state [s]
      #-input: system input at this sample
        #-acceleration: acceleration input to the vehicle [m/s^2]
        #-steerAngleSingleTrack: single-track steer angle input [rad]
    #-curvilinearAbscissa: numpy array of curvilinear abscissa values
    #-length: total length of the trajectory (same as last item of curvilinearAbscissa vector)
  
  # In case you need some vehicle parameters, you can look at vrep.vehicle_parameters dictionary, e.g. vrep.vehicle_parameters['maxSpeedForward']
  
  # In python you can do the following to get a e.g. a list of x values (list comprehension)
  # x_vec = [s.state.x for s in trajectory.samples]
  
  
  # This creates a breakpoint. 
  # This helps you stepping through the code line by line
  # Quick pdb cheat sheet (execute these in debug mode):
  #   l(ist): list 11 lines of code around the current line
  #   n(ext): execute the current statement (step over)
  #   c(ontinue): continue execution until a breakpoint is encountered
  # Have a look at http://appletree.or.kr/quick_reference_cards/Python/Python%20Debugger%20Cheatsheet.pdf for more commands
  # TODO: Remove if you want to run the code seamlessly.
  #Tracer()()
  
    
  # Define your weight vector for the cost terms
  # TODO: Fill in the weight vector here
  nCostTerms = 3
  weights = np.ones( shape=(1,nCostTerms) )
  weights[0,0] = 0.3
  weights[0,1] = 0.5
  weights[0,2] = 0.2
  
  # Initialize some variables
  bestTrajectory = None
  costs = np.inf*np.ones(shape=(len(trajectorySet), weights.size))

  # Compute cost for every trajectory
  for cnt,(trajectory,collisionFlag,closestDistanceToStaticObstacles) in enumerate(trajectorySet):
    if not np.isinf(collisionFlag): # only for non-colliding trajectories
      
      # TODO: Fill in code here
      # NOTE: If you compute integrals (e.g. with euler method), consider subsampling and approximating the integral with fewer support points to speed up computations.
      # E.g. do integral = integrand[::2]*integration_variable[::2] to only take every second support point.
      # ...
      
      #costs[cnt, :] = np.zeros(shape=(weights.size,)) # TODO: fill this in correctly
      
      num_samples = len(trajectory.samples)
      sum_speeds   = 0
      sum_y_offset = 0
      sum_dist         = 0
      for i in range(0,num_samples):
      	temp1=trajectory.samples[i]
        curr_speed = temp1.state.speed
        curr_y_offset   = temp1.state.y
	sum_speeds = sum_speeds + curr_speed
        sum_y_offset    = sum_y_offset + curr_y_offset
        curr_dist = pathDistanceMap.getByWorldCoordinates(temp1.state.x, temp1.state.y)
        sum_dist = sum_dist + curr_dist

      #Tracer()()
      #num_samples1 = len(closestDistanceToStaticObstacles.samples)
      #min_dist = 1000
      #for i in range(0,num_samples1):
        #temp2 = closestDistanceToStaticObstacles.

      #dist = pathDistanceMap.getByWorldCoordinates(trajectory[cnt].np[].state.x, s.state.y)
      #print (trajectory)
      
      #costs[cnt, :] = np.ones(shape=(weights.size,))
      costs[cnt, 0] = 1/(sum_speeds+0.001)
      costs[cnt, 1] = closestDistanceToStaticObstacles
      costs[cnt, 2] = sum_dist #1/(sum_y_offset+0.001)
      #print (costs[cnt, 0])
      #print (costs[cnt, 1])
      #print (costs[cnt, 2])
      

    
  # normalize each cost term
  for idx in range(costs.shape[1]):
    valid = np.isinf(costs[:,idx]) == False
    if np.any(valid): # NOTE: happens when all edges are in collision or trajectory set is empty
      maxCosts = np.abs(np.max(costs[valid,idx]))
      if maxCosts != 0.:
        costs[valid,idx] = (costs[valid,idx] - np.min(costs[valid,idx]))/maxCosts
    
  # compute weighted sum of individual cost terms
  weightedCosts = np.sum(weights*costs, axis=1)
  
  # Find minimum cost
  minIdx = np.argmin( weightedCosts )
  bestTrajectory = trajectorySet[minIdx][0]
  
  # Deal with all edges in collision
  if np.isinf(weightedCosts[minIdx]):
    # Create empty trajectory. That is interpreted  as an emergency brake on V-Rep side
    bestTrajectory = vrep.lpp.TrajectoryVCharge()
    print "All edges in collision -> emergency brake"
    
    
  return bestTrajectory

  
  
  

  
  
  
  
  
  
  
  

  

# **** #
# MAIN
# **** #


connectionAddress = '127.0.0.1'
port=19997 # On this port runs a continuous V-Rep remote API server. We can connect without the simulation running (port has to be entered into remoteApiConnections.txt in the V-Rep installation folder)
robotId=0 # the robot number (robot#0) that we want to control


#Establish remote connection to V-Rep
vrep.simxFinish(-1) # just in case, close all opened connections
clientID=vrep.simxStart(connectionAddress,port,waitUntilConnected=True,doNotReconnectOnceDisconnected=True,timeOutInMs=5000,commThreadCycleInMs=50)
if clientID is -1:
    raise RuntimeError('Could not establish connection to V-Rep, was V-Rep launched before? V-Rep simulation does not have to be started, but the application has to be running.')
vrep.simxAddStatusbarMessage(clientID, 'Remote connection successfully established from python from address {0}, port {1}. Controlling robot #{2}'.format(connectionAddress, port, robotId), vrep.simx_opmode_oneshot_wait)

# Start the simulation
print "Starting the simulation..."
vrep.simxStartSimulation(clientID, vrep.simx_opmode_oneshot_wait)
vrep.simxGetFloatSignal(clientID, '', vrep.vrep.simx_opmode_oneshot_wait)
vrep.simxGetFloatSignal(clientID, '', vrep.vrep.simx_opmode_oneshot_wait)
vrep.simxGetFloatSignal(clientID, '', vrep.vrep.simx_opmode_oneshot_wait)

# Create the python interface
print "Creating python interface..."
iface = vrep.VrepPython(clientID, parameters=parameters, robotId=robotId)

# Enter infinite control loop
print "Entering planning/control loop..."
doProcess = True
lastSolution = None

while doProcess:
  
  # Check whether connection is still alive
  if vrep.simxGetConnectionId(clientID) == -1:
    raise RuntimeError("Lost connection to V-Rep")

  # Gets the latest data from V-Rep and computes trajectory set
  doProcess = iface.computeTrajectorySet()

  # Compute your custom trajectory scores and choose the best one
  start = time.time()
  bestTrajectory = getBestTrajectory(iface.getTrajectorySet(), iface.getPathDistanceMap(), lastSolution)
  print "Custom cost function computation took {0} ms".format((time.time() - start)*1e3)
  lastSolution = bestTrajectory

  # Send vehicle commands, respectively trajectory to V-Rep
  iface.sendTrajectory(bestTrajectory)
  
  # Plotting
  iface.plotTrajectory(bestTrajectory)
  iface.plotDynamicObjectPredictions(iface._dynamicObjectPredictions)

  if parameters['plotTrajectorySetInVrep']:
    iface.plotTrajectorySet(iface.getTrajectorySet())
  
  if parameters['plotInPython']:
    fig = pl.figure(0, figsize=(15,15))
    iface.plotPython(bestTrajectory, plotPathDistanceMap=True, plotObstacleDistanceMap=False, figNum=0)
