import os
from vrep import *

import local_planning_python as lpp
import bt_transform_python as trafo
import itertools
import collections
import numpy as np
import cv
import ctypes
from warnings import warn
from math import atan,atan2,cos,sin,hypot
import sys
import time


import pylab as pl

vrep_return_codes = {0x000000: 'simx_error_noerror', 
                     0x000001: 'simx_error_novalue_flag',
                     0x000002: 'simx_error_timeout_flag',
                     0x000004: 'simx_error_illegal_opmode_flag',
                     0x000008: 'simx_error_remote_error_flag',
                     0x000010: 'simx_error_split_progress_flag',
                     0x000020: 'simx_error_local_error_flag',
                     0x000040: 'simx_error_initialize_error_flag'}

# has to be consistent with what is set in V-Rep
signalNames = {'LocalGridSensor' : 'localGridSensor',
               'OdometryFrame': 'odometryFrame',
               'VehicleFrame' : 'vehicleFrame',
               'MapFrame' : 'mapFrame',
               'DynamicObject': 'dynamicObject'}

vehicle_parameters = {'maxSteerAngleRad' : 0.64, 
                      'maxSteerSpeedRadPerS' : 0.57, 
                      'maxAcceleration': 1.5, 
                      'maxDeceleration': 3.0,
                      'interAxleDistance': 2.57,
                      'maxSpeedForward': 2.6, 
                      'maxSpeedBackward': 1.0,
                      'lengthMeters' : 4.199,
                      'widthMeters' : 2.0,
                      'rearToReferenceMeters' : 0.753}
                      
controller_parameters = {'ky': 0.1, 
                         'ktheta': 0.1, 
                         'steerAngleSmoothingTimeConstant': 0.1, 
                         'dyLimit': 3.0, 
                         'useFeedForward': True, 
                         'useGainScheduling': True,
                         'minimumTrackableVehicleSpeed': 0.2, 
                         'keepMovingAcceleration': 0.2}
                         
sampling_parameters = {'nLateralOffsetSamples': 11,
                       'maxLateralOffset' : 4.0,
                       'nSpeedSamples': 7,
                       'nVelocityControllerPGainSamples': 3,
                       'velocityControllerPGainMin': 0.3,
                       'velocityControllerPGainMax': 1.0}
                       
  
class VrepException(Exception):
  pass
           
class VrepWarning(Warning):
  pass

class TransformationWarning(Warning):
  pass

def checkVrepReturnCode(code, desc=None):
  ''' Raises exception if V-rep return code is something else then simx_error_noerror or simx_error_novalue_flag
  '''
  if not code == vrep.simx_error_noerror:
    if code == vrep.simx_error_novalue_flag: # NOTE: this does not have to be an error
      if not desc is None:
        warnString = desc + ": V-Rep returned 'simx_error_novalue_flag'"
      warn(warnString, VrepWarning)
    else:
      errors = []
      for retCode,name in vrep_return_codes.iteritems():
        if retCode & code != 0:
          errors.append(name)
      exceptionString = "V-Rep signaled error(s) ["+ ','.join(errors)+ "]"
      if not desc is None:
        exceptionString = desc + ": " + exceptionString
      raise VrepException(exceptionString)
  
def check(returnCode, data, desc=None):
  ''' Raises exception if V-rep return code is something else then simx_error_noerror or data is empty
  '''
  checkVrepReturnCode(returnCode,desc)
  if isinstance(data, collections.Sequence) and not data:
    exceptionString = "Empty data returned"
    if not desc is None:
      exceptionString = desc + ": " + exceptionString
    raise VrepException(exceptionString)
  
def convertPath(path, referenceSpeed=vehicle_parameters['maxSpeedForward']):
  
  if not path.shape[0] == 4:
    raise RuntimeError("Invalid size of array representing the reference path, expected 4xN array (x,y,head,curv)")
  
  referencePathLocalPlanner = lpp.TrajectoryVCharge()
  for i in range(0,path.shape[1]):
    #if i!=path.shape[1]-1:
      #head = atan2(path[1,i+1]-path[1,i], path[0,i+1]-path[0,i])
    #else:
      #head = atan2(path[1,i]-path[1,i-1], path[0,i]-path[0,i-1])
    s = lpp.StateVCharge(path[0,i], path[1,i], path[2,i], referenceSpeed, 0.0, 0.0, 0.0, 0.0)
    steerAngleSingleTrackRad = atan(vehicle_parameters['interAxleDistance']*path[3,i])
    #print "steerAngleSingleTrackRad[{0}] = {1}".format(i, steerAngleSingleTrackRad)
    u = lpp.SystemInputVCharge(steerAngleSingleTrackRad, 0.0) # feed-forwards
    item = lpp.TrajectoryItemVCharge(s,u)
    referencePathLocalPlanner.samples.append(item)
    
  return referencePathLocalPlanner

  
class VrepPython(object):
  def __init__(self, clientId, robotId, parameters):
    
    # check inputs
    if not type(clientId) is int:
      raise TypeError( "Invalid input of {0} for clientId, has to be {1}".format(type(clientId), type(int) ) )
    if clientId<0:
      raise ValueError( "{0} is an invalid client id for the V-Rep remote API. Looks like something went wrong when establishing the connection".format(clientId) )
    if not type(robotId) is int:
      raise TypeError( "Invalid input of {0} for robotId, has to be {1}".format(type(robotId), type(int) ) )
    if not robotId>=0:
      raise ValueError( "Invalid input of {0} for robotId, has to larger or equal to zero".format(robotId) )
    
    if not 'useAngularRateForMotionPredictions' in parameters.keys():
      raise TypeError( "parameters input is missing dictionary key 'useAngularRateForMotionPredictions'" )
    if not type(parameters['useAngularRateForMotionPredictions']) is bool:
      raise TypeError( "parameters['useAngularRateForMotionPredictions'] has to be a boolean value" )
    
    self._clientId = clientId
    self._robotId = str(robotId)
    self._robotSuffix = str(robotId) if robotId != 0 else ''
    self._parameters = parameters
    
    # create the directory for the logfile
    thisDir = os.path.dirname(os.path.realpath(__file__))
    logDir = os.path.join(thisDir, "../../logs")
    if not os.path.isdir(logDir):
      os.makedirs(logDir)
    self._logFile = os.path.join(logDir, "success.log")
    
    # Get V-Rep handles
    self._vrepHandles = {}
    
    handlesToRetrieve = {'localMapSensor': signalNames["LocalGridSensor"]+self._robotSuffix,
                         'odometryFrame': signalNames["OdometryFrame"]+self._robotSuffix,
                         'vehicleFrame': signalNames["VehicleFrame"]+self._robotSuffix,
                         'mapFrame': signalNames["MapFrame"]+self._robotSuffix }
                             
    for insertName,handleName in handlesToRetrieve.iteritems():
      returnCode,handle=vrep.simxGetObjectHandle(self._clientId, handleName, vrep.simx_opmode_oneshot_wait)
      checkVrepReturnCode(returnCode, "Could not get handle of object '{0}'".format(handleName))
      self._vrepHandles[insertName] = handle
    
    self._vrepHandles['vrepFrame'] = -1 # global V-Rep frame, this is not an object
    
    # Set up transformation server
    self._tfServer = trafo.TfServerIntegerStamp('vrepFrame', 100)
    self._tfServer.addCoordSys('vrepFrame','odometryFrame')
    self._tfServer.addCoordSys('odometryFrame','mapFrame')
    self._tfServer.addCoordSys('odometryFrame','vehicleFrame')

    # create the map that the local planner holds
    self._localPlannerMap = self.initializeLocalMap()
    
    # Initially set members to None
    self._referencePath = None
    self._trajectorySet = None
    
    # create the system model
    systemModel = lpp.SystemModelVCharge();
    systemModel.configure (vehicle_parameters['maxSteerAngleRad'], vehicle_parameters['maxSteerSpeedRadPerS'], vehicle_parameters['maxAcceleration'], vehicle_parameters['maxDeceleration'], vehicle_parameters['interAxleDistance'], vehicle_parameters['maxSpeedForward'], vehicle_parameters['maxSpeedBackward'])

    #create the controller
    controller = lpp.ControllerVCharge();
    controller.configure(controller_parameters['ky'], controller_parameters['ktheta'], vehicle_parameters['interAxleDistance'], vehicle_parameters['maxSpeedForward'], vehicle_parameters['maxSteerAngleRad'], vehicle_parameters['maxAcceleration'], vehicle_parameters['maxDeceleration'], controller_parameters['minimumTrackableVehicleSpeed'], controller_parameters['keepMovingAcceleration'], controller_parameters['steerAngleSmoothingTimeConstant'], controller_parameters['dyLimit'], controller_parameters['useFeedForward'], controller_parameters['useGainScheduling'])
    controller.configureGainScheduling(0.05, 0.2, 0.4, 1.0)

    #create the sampling scheme
    self._samplingScheme = lpp.SamplingVCharge()
    self._samplingScheme.initialize(sampling_parameters['nLateralOffsetSamples'], sampling_parameters['maxLateralOffset'], sampling_parameters['nSpeedSamples'], vehicle_parameters['maxSpeedForward'], sampling_parameters['nVelocityControllerPGainSamples'], sampling_parameters['velocityControllerPGainMin'], sampling_parameters['velocityControllerPGainMax'])

    # Planner parameters
    self._plannerParameters = lpp.LocalPlannerParameters()
    
    self._plannerParameters.vehicle.lengthMeters = vehicle_parameters['lengthMeters']
    self._plannerParameters.vehicle.widthMeters = vehicle_parameters['widthMeters']
    self._plannerParameters.vehicle.rearToReferenceMeters = vehicle_parameters['rearToReferenceMeters']
    self._plannerParameters.vehicle.egoRadius = np.hypot(self._plannerParameters.vehicle.widthMeters/2., self._plannerParameters.vehicle.lengthMeters/2.)
    
    self._plannerParameters.collisionChecker.maxNDisks = 10
    self._plannerParameters.collisionChecker.maxOverhangSides = 0.2
    self._plannerParameters.collisionChecker.maxOverhangFrontRear = 0.4
    self._plannerParameters.collisionChecker.safetyMargin = 0.0
    
    self._plannerParameters.controller.pathLookAhead = 0.2
    self._plannerParameters.controller.speedDependantLookAheadFactor = 0.3
    
    self._plannerParameters.planner.allowUnknownCellTraversal = False
    self._plannerParameters.planner.cycleTimeCompensation = False # NOTE: This does not work since we would have to feed back the computed trajectory from python in order to obtain the system inputs for the vehicle
    self._plannerParameters.planner.doCollisionChecking = True
    self._plannerParameters.planner.doCollisionCheckingDynamicObjects = True
    self._plannerParameters.planner.noCostComputation = True
    self._plannerParameters.planner.dynamicObstacleMinimumTimeGap = 3.0
    self._plannerParameters.planner.integratedLateralPathDistanceWeight = 0.0
    self._plannerParameters.planner.integratedSpeedErrorWeight = 0.0
    self._plannerParameters.planner.mapFreeVal = 0
    self._plannerParameters.planner.mapOccupiedVal = 100
    self._plannerParameters.planner.mapUnknownVal = -1
    self._plannerParameters.planner.maxPlanningTimeSec = 0.4
    self._plannerParameters.planner.numberOfCostIntegralSupportPoints = 100 # NOTE: does not matter if noCostComputation==True
    self._plannerParameters.planner.obstacleDistancePenalizationThreshold = 1000.0 # NOTE: to avoid upper threshold
    self._plannerParameters.planner.obstacleDistancePenalizationWeight = 0.0
    self._plannerParameters.planner.oscillationPreventionRelativeCostImprovementThreshold = 0.0
    self._plannerParameters.planner.oscillationPreventionSuboptimalityIntegrationFactor = 0.0
    self._plannerParameters.planner.oscillationPreventionWeight = 0.0
    self._plannerParameters.planner.terminalLateralPathDistanceWeight = 0.0
    self._plannerParameters.planner.terminalSpeedErrorWeight = 0.0
    self._plannerParameters.planner.terminalStateErrorWeight = 0.0
    self._plannerParameters.planner.integratedControlEffortWeight = 0.0
    self._plannerParameters.planner.traceBackLevel = 0
    self._plannerParameters.planner.treeDepth = 1
    
    self._plannerParameters.planner.distanceMapSizeXInCells = 512
    self._plannerParameters.planner.distanceMapSizeYInCells = 512
    self._plannerParameters.planner.distanceMapResolutionMeters = 0.1
    self._plannerParameters.planner.trappedStateDetection = 'none'
    
    self._plannerParameters.simulation.simTimeEdgeSec = 8.0
    self._plannerParameters.simulation.simTimeGranularity = 0.1
    
    self._localPlanner = lpp.LocalPlannerVCharge()
    success = self._localPlanner.init(self._localPlannerMap, self._plannerParameters)
    if not success:
      raise RuntimeError("Could not intialize the local planner")
    self._localPlanner.setController(controller)
    self._localPlanner.setSystemModel(systemModel)
    self._localPlanner.setSamplingScheme(self._samplingScheme)
    
    self._localPlannerReturnCode = None

    
    # initialize the V-Rep sockets for communication
    print "Opening V-Rep communication sockets..."
    self.updateTfServer(vrep_opmode=vrep.simx_opmode_streaming)
    self.getVehicleStates(vrep_opmode=vrep.simx_opmode_streaming)
    self.getLocalMap(vrep_opmode=vrep.simx_opmode_streaming)
    #self.getLocalMap(vrep_opmode=vrep.simx_opmode_streaming_split+4000) # NOTE: split mode seems to corrupt data sometimes

    # update the tf server in wait mode, so we can be sure we have at least one valid transformation
    self.updateTfServer(vrep_opmode=vrep.simx_opmode_buffer, checkReturnCode = False)
    i = 0
    while not self._tfServer.haveAllFramesAtLeastOneTransform():
      if i == 0:
        print "Waiting for transformations from V-Rep..."
      self.updateTfServer(vrep_opmode=vrep.simx_opmode_buffer, checkReturnCode = False)
      i+=1

    # open pipes for dynamic objects in streaming mode
    self._maxNumDynamicObjects = 30
    for i in range(self._maxNumDynamicObjects):
      _,_=vrep.simxGetStringSignal(self._clientId,signalNames['DynamicObject']+str(i),vrep.simx_opmode_streaming);
      
    # get reference path
    print "Retrieving reference path..."
    returnCode,data=vrep.simxGetStringSignal(self._clientId,'referencePath'+self._robotId,vrep.simx_opmode_oneshot_wait);
    check(returnCode, data, "Could not get reference path")
    if len(data)%4 != 0:
      raise RuntimeError("Invalid array size of reference path data, expecting multiple of 4 (x,y,head,curv)")
    self._referencePath = np.array(vrep.simxUnpackFloats(data))
    self._referencePath = np.reshape(self._referencePath, (int(self._referencePath.size/4), 4)).transpose()
    
    # Store initial time to evaluate the travel time later
    self._startTime = self.getSimulationTime('s')
        
    # set logging level of local planner
    lpp.setLoggingLevel(lpp.LoggingLevel.Warn)
    
    print "Successfully initialized!"
    
  def getTrajectorySet(self):
    return self._trajectorySet

  def getReferencePath(self):
    return self._referencePath
    
  def plotTrajectory(self, trajectory):
    ''' Sends a trajectory to plot to V-Rep
    '''
    currentStampMicroSec = self.getSimulationTime('us')
    path = np.array( [[s.state.x for s in trajectory.samples], [s.state.y for s in trajectory.samples] ] )
    path = self._tfServer.transformPathXY(path, currentStampMicroSec, "mapFrame", "vrepFrame")
    packedData = vrep.simxPackFloats(path.flatten())
    rawBytes = self.stringBufferToRawBytes(packedData)
    vrep.simxSetStringSignal(self._clientId,'Plotting_SolutionTrajectory'+self._robotId,rawBytes,vrep.simx_opmode_oneshot);
    
  def plotDynamicObjectPredictions(self, dynamicObjectPredictions):
    ''' Sends a trajectory to plot to V-Rep
    '''
    packedData = ''
    currentStampMicroSec = self.getSimulationTime('us')
    for dynObs in dynamicObjectPredictions:
      trajectory = dynObs.getTrajectory()
      path = np.array( [[s.x for s in trajectory], [s.y for s in trajectory]] )
      path = self._tfServer.transformPathXY(path, currentStampMicroSec, "mapFrame", "vrepFrame")
      packedData += vrep.simxPackFloats( np.hstack( (path.flatten(), np.nan) ) )
      
    rawBytes = self.stringBufferToRawBytes(packedData)
    vrep.simxSetStringSignal(self._clientId,'Plotting_DynamicObjectPredictions'+self._robotId,rawBytes,vrep.simx_opmode_oneshot);
    
  def checkTerminationCriterion(self):
    ''' Checks whether the goal is reached
    '''
    return self._localPlanner.percentCompleted(self._currentStateMapRF) > 0.999

  def plotTrajectorySet(self,trajectorySet):
    ''' Sends a trajectory set to plot to V-Rep
    '''
    if not isinstance(trajectorySet, collections.Sequence):
      raise TypeError("Invalid type {0} of trajectory set".format(type(trajectorySet)))
    
    currentStampMicroSec = self.getSimulationTime('us')
    packedDataColliding = ''
    packedDataNonColliding = ''
    for trajectory,collisionFlag,closestDistanceToStaticObstacles in trajectorySet:
      path = np.array( [[s.state.x for s in trajectory.samples], [s.state.y for s in trajectory.samples] ] )
      path = self._tfServer.transformPathXY(path, currentStampMicroSec, "mapFrame", "vrepFrame")
      packedData = vrep.simxPackFloats( np.hstack( (path.flatten(), np.nan) ) )
      if np.isinf(collisionFlag):
        packedDataColliding += packedData
      else:
        packedDataNonColliding += packedData
    
    rawBytesColliding = self.stringBufferToRawBytes(packedDataColliding)
    rawBytesNonColliding = self.stringBufferToRawBytes(packedDataNonColliding) 
    vrep.simxSetStringSignal(self._clientId,'Plotting_TrajectorySet_Colliding'+self._robotId,rawBytesColliding,vrep.simx_opmode_oneshot);
    vrep.simxSetStringSignal(self._clientId,'Plotting_TrajectorySet_NonColliding'+self._robotId,rawBytesNonColliding,vrep.simx_opmode_oneshot);
    
  def initializeLocalMap(self):
    ''' Initializes the local map data structure for the local planner
    '''
    
    # get first image to determine the resolution
    returnCode,resolution,image=vrep.simxGetVisionSensorImage(self._clientId, self._vrepHandles['localMapSensor'], 1, vrep.simx_opmode_oneshot_wait)
    check(returnCode, image, "Could not get vision sensor image")
    
    # Get size of the map in meters
    returnCode,orthographicProjectionSize=vrep.simxGetObjectFloatParameter(self._clientId, self._vrepHandles['localMapSensor'], 1005, vrep.simx_opmode_oneshot_wait)
    checkVrepReturnCode(returnCode, "Could not get float parameter 1005 from local map sensor (orthographic projection size)")
      
    localPlannerMap = lpp.UCharMap()
    occupancyMapParameters = lpp.MapParameters()
    occupancyMapParameters.mapResolutionMeters = orthographicProjectionSize/resolution[0]
    occupancyMapParameters.mapSizeXCells=resolution[0]
    occupancyMapParameters.mapSizeYCells=resolution[1]
    occupancyMapParameters.mapOriginXMeters = 0.0 # NOTE: in mapFrame
    occupancyMapParameters.mapOriginYMeters = 0.0 # NOTE: in mapFrame
    localPlannerMap.init(occupancyMapParameters)

    return localPlannerMap
    
  def getTransform(self, frameFrom, frameTo, vrep_opmode=vrep.simx_opmode_buffer, checkReturnCode=True):
    ''' Retrieves a transformation from V-Rep
    '''
    
     # NOTE: Only works in streaming mode with V-Rep version >= 3.1.2 (http://www.forum.coppeliarobotics.com/viewtopic.php?f=9&t=1819)
    returnCode,position=vrep.simxGetObjectPosition(self._clientId, self._vrepHandles[frameFrom], self._vrepHandles[frameTo], vrep_opmode)
    if vrep_opmode != vrep.simx_opmode_streaming and checkReturnCode:
      check(returnCode, position, "Could not get object position of {0} with respect to {1}".format(frameFrom, frameTo))
      
    returnCode,euler=vrep.simxGetObjectOrientation(self._clientId, self._vrepHandles[frameFrom], self._vrepHandles[frameTo], vrep_opmode)
    if vrep_opmode != vrep.simx_opmode_streaming and checkReturnCode:
      check(returnCode, euler, "Could not get object orientation of {0} with respect to {1}".format(frameFrom, frameTo))
      
    stamp = vrep.simxGetLastCmdTime(self._clientId)
    
    if vrep_opmode != vrep.simx_opmode_streaming:
      return np.array(position),np.array(euler),stamp
    else:
      return None,None,None
    
  @staticmethod
  def vrepStampToMicrosec(stamp):
    ''' V-Rep returns its timestamps in milliseconds
    '''
    return int(stamp*1e3)
    
  @staticmethod
  def vrepStampToSec(stamp):
    ''' V-Rep returns its timestamps in milliseconds
    '''
    return stamp/1e3
    
  def updateTfServer(self, vrep_opmode=vrep.simx_opmode_buffer, checkReturnCode=True):
    ''' Retrieves the latest localization information from V-Rep and updates the python-side transformation server
    '''

    position,euler,stamp = self.getTransform('odometryFrame', 'vrepFrame', vrep_opmode, checkReturnCode)
    if vrep_opmode != vrep.simx_opmode_streaming:
      stampMicrosec = self.vrepStampToMicrosec(stamp)
      if not self._tfServer.doesTimestampExist(stampMicrosec, 'odometryFrame'):
        success = self._tfServer.updateCoordSys('odometryFrame', position, euler, stampMicrosec, False)
        if not success:
          warn("Could not update transformation server", TransformationWarning)
    
    position,euler,stamp = self.getTransform('mapFrame', 'odometryFrame', vrep_opmode, checkReturnCode)
    if vrep_opmode != vrep.simx_opmode_streaming:
      stampMicrosec = self.vrepStampToMicrosec(stamp)
      if not self._tfServer.doesTimestampExist(stampMicrosec, 'mapFrame'):
        success = self._tfServer.updateCoordSys('mapFrame', position, euler, self.vrepStampToMicrosec(stamp), False)
        if not success:
          warn("Could not update transformation server", TransformationWarning)
    
    position,euler,stamp = self.getTransform('vehicleFrame', 'odometryFrame', vrep_opmode, checkReturnCode)
    if vrep_opmode != vrep.simx_opmode_streaming:
      stampMicrosec = self.vrepStampToMicrosec(stamp)
      if not self._tfServer.doesTimestampExist(stampMicrosec, 'vehicleFrame'):
        success = self._tfServer.updateCoordSys('vehicleFrame', position, euler, self.vrepStampToMicrosec(stamp), False)
        if not success:
          warn("Could not update transformation server", TransformationWarning)


  def getVehicleStates(self, vrep_opmode=vrep.simx_opmode_buffer):
    ''' Fetches steering wheel angle, steering speed, vehicle speed and vehicle acceleration from V-rep
    '''
    
    steerAngle = None
    steerSpeed = None
    speed = None
    acc = None
    
    # get current steering wheel angle
    returnCode,data=vrep.simxGetFloatSignal(self._clientId,'Car_SteeringWheelAngle_Current'+self._robotId,vrep_opmode);
    if vrep_opmode!=vrep.simx_opmode_streaming:
      check(returnCode, data, "Could not get current steering angle of vehicle")
      steerAngle = data
    
    # get current steering wheel speed
    returnCode,data=vrep.simxGetFloatSignal(self._clientId,'Car_SteeringWheelSpeed_Current'+self._robotId,vrep_opmode);
    if vrep_opmode!=vrep.simx_opmode_streaming:
      check(returnCode, data, "Could not get current steering speed of vehicle")
      steerSpeed = data
      
    # get current speed
    returnCode,data=vrep.simxGetFloatSignal(self._clientId,'Car_VehicleSpeed_Current'+self._robotId,vrep_opmode);
    if vrep_opmode!=vrep.simx_opmode_streaming:
      check(returnCode, data, "Could not get current vehicle speed")
      speed = data
    
    # get current acceleration
    returnCode,data=vrep.simxGetFloatSignal(self._clientId,'Car_VehicleAcceleration_Current'+self._robotId,vrep_opmode);
    if vrep_opmode!=vrep.simx_opmode_streaming:
      check(returnCode, data, "Could not get current vehicle acceleration")
      acc = data
    
    return steerAngle,steerSpeed,speed,acc
    
  def getLocalMap(self, vrep_opmode=vrep.simx_opmode_buffer):
    ''' Reads local map vision sensor
    '''
    returnCode,resolution,image=vrep.simxGetVisionSensorImage(self._clientId,self._vrepHandles['localMapSensor'],1,vrep_opmode)
    if vrep_opmode != vrep.simx_opmode_streaming and vrep_opmode != vrep.simx_opmode_streaming_split+4000:
      check(returnCode, image, "Could not get local map image")
      if len(image) > 0:
        return np.reshape(np.array(image, dtype=np.uint8), resolution)
    
  def getDynamicObjects(self, vrep_opmode=vrep.simx_opmode_buffer):
    ''' Reads dynamic objects from V-Rep socket
    '''
    dynObsData = []
    for i in range(self._maxNumDynamicObjects):
      returnCodeDynObs,data=vrep.simxGetStringSignal(self._clientId,signalNames['DynamicObject']+str(i),vrep_opmode);
      if data and returnCodeDynObs == vrep.simx_error_noerror:
        dynObsData.append(data)
    return dynObsData
    
  def computeTrajectorySet(self):
    ''' Retrieves latest data from V-Rep and updates the information in the local planner
    '''
    
    # Get localization updates
    self.updateTfServer(vrep_opmode=vrep.simx_opmode_buffer)
    
    currentStateVRF = lpp.StateVCharge(0.0, 0.0, 0.0)
    currentStateVRF.steerAngle, currentStateVRF.steerSpeed, currentStateVRF.speed, currentStateVRF.acceleration = self.getVehicleStates(vrep_opmode=vrep.simx_opmode_buffer)
      
    # get local map
    npArr = self.getLocalMap(vrep_opmode=vrep.simx_opmode_buffer)
    if not npArr is None:
      npArr[npArr == 255] = self._plannerParameters.planner.mapOccupiedVal
      self._localPlannerMap.setByNumpyArray(npArr)
      
    # get dynamic objects
    dynObsData = self.getDynamicObjects(vrep_opmode=vrep.simx_opmode_buffer)

    self._dynamicObjectPredictions = self.generateDynamicObjectPredictions(dynObsData, useAngularRate=self._parameters['useAngularRateForMotionPredictions'])

    
    # transform everything into the map frame
    currentStampSec = self.getSimulationTime('s')
    position_map,euler_map=self._tfServer.transformPoseAtTime(np.array([0.,0.,0.]), np.array([0.,0.,0.]), self.getSimulationTime('us'), 'vehicleFrame', 'mapFrame')
    self._currentStateMapRF = currentStateVRF
    self._currentStateMapRF.x = position_map[0]
    self._currentStateMapRF.y = position_map[1]
    self._currentStateMapRF.heading = euler_map[2]
    self._currentStateMapRF.time = currentStampSec
        
    self._referencePathMapRF = self._tfServer.transformPathXYHeading(self._referencePath[0:3,:], int(currentStampSec*1e6), "vrepFrame", "mapFrame")
    self._referencePathMapRF = np.vstack( (self._referencePathMapRF, self._referencePath[3,:]) )
    self._referencePathMapRF = convertPath(self._referencePathMapRF, vehicle_parameters['maxSpeedForward'])
    
    # check termination criterion
    if self.checkTerminationCriterion():
      travelTime = self.getSimulationTime('s') - self._startTime
      print "++++++++++++++++++++++++++++++++++++++++"
      print "Successfully reached goal in {0} seconds".format(travelTime)
      print "++++++++++++++++++++++++++++++++++++++++"
      vrep.simxStopSimulation(self._clientId, vrep.simx_opmode_oneshot)
      with open(self._logFile, "a") as f:
        f.write("{0} s\n".format(travelTime))
      return False

    # Update the information in the local planner
    success = self._localPlanner.setReferenceTrajectory(self._referencePathMapRF)
    if not success:
      raise RuntimeError("Could not set reference path in local planner, watch out for errors in the console reported by the local planning module")
    self._localPlanner.updateObstacleMap()
    self._localPlanner.setDynamicObstacles(self._dynamicObjectPredictions)
    
    # compute trajectory set
    self._solutionTrajectory = lpp.TrajectoryVCharge()
    samplingParameter = lpp.SamplingParameter3()
    start = time.time()
    self._localPlannerReturnCode = self._localPlanner.computeSolutionTrajectory(self._currentStateMapRF, self._solutionTrajectory, samplingParameter);
    print "Local planner return code: {0}, computation time: {1:3f} ms".format(self._localPlannerReturnCode, (time.time() - start)*1e3)
    self._trajectorySet = self._localPlanner.getTrajectorySet()      
    
    # modify the cost terms computed by the local planner
    for idx in range(len(self._trajectorySet)):
      if self._trajectorySet[idx][1] < sys.float_info.max:
        self._trajectorySet[idx] = (self._trajectorySet[idx][0], -1.0, self._trajectorySet[idx][2])
      else:
        self._trajectorySet[idx] = (self._trajectorySet[idx][0], np.inf, self._trajectorySet[idx][2])
    
    return True
    
  def computeClosestDistanceToStaticObstacles(self, pose):
    ''' Computes the distance of vehicle at pose "pose" to the closest static obstacle
    '''
    return self._localPlanner.computeClosestDistanceToStaticObstacles(pose)
    
  def plotPython(self, solutionTrajectory, plotObstacleDistanceMap=True, plotPathDistanceMap=False, figNum=0):

    fig = pl.figure(figNum)
    fig.clf()
    ax = pl.gca()  

    # Plot of obstacle distance map
    if plotObstacleDistanceMap:
      obstacleDistanceMap = self.getObstacleDistanceMap()
      ax.imshow(obstacleDistanceMap.getNumpyArray(), extent=[obstacleDistanceMap.getOriginX(),
                obstacleDistanceMap.getOriginX()+obstacleDistanceMap.getResolution()*obstacleDistanceMap.getSizeX(),
                obstacleDistanceMap.getOriginY(),
                obstacleDistanceMap.getOriginY()+obstacleDistanceMap.getResolution()*obstacleDistanceMap.getSizeY()], origin='lower', cmap='jet', interpolation='nearest', alpha=0.3)
                
    # Plot of path distance map
    if plotPathDistanceMap:
      pathDistanceMap = self.getPathDistanceMap()
      ax.imshow(pathDistanceMap.getNumpyArray(), extent=[pathDistanceMap.getOriginX(),
                pathDistanceMap.getOriginX()+pathDistanceMap.getResolution()*pathDistanceMap.getSizeX(),
                pathDistanceMap.getOriginY(),
                pathDistanceMap.getOriginY()+pathDistanceMap.getResolution()*pathDistanceMap.getSizeY()], origin='lower', cmap='jet', interpolation='nearest', alpha=0.3)
    
            
    # Plot of gridmap
    gridImage = self._localPlannerMap.getNumpyArray()
    maskedGridImage = np.ma.masked_where(gridImage == self._plannerParameters.planner.mapFreeVal, gridImage)
    ax.imshow(maskedGridImage, extent=[self._localPlannerMap.getOriginX(),
               self._localPlannerMap.getOriginX()+self._localPlannerMap.getResolution()*self._localPlannerMap.getSizeX(),
               self._localPlannerMap.getOriginY(),
               self._localPlannerMap.getOriginY()+self._localPlannerMap.getResolution()*self._localPlannerMap.getSizeY()], origin='lower', cmap='Greys', interpolation='nearest', alpha=1.0)

    # plot current pose
    ax.plot(self._currentStateMapRF.x, self._currentStateMapRF.y, 'og', label='ego')
                      
    #plot reference path
    ax.plot([s.state.x for s in self._referencePathMapRF.samples], [s.state.y for s in self._referencePathMapRF.samples], 'r', label='reference')
    
    # plot trajectory set
    if not self._trajectorySet is None:
      costs = np.array([collisionFlag for _,collisionFlag,_ in self._trajectorySet])
      costs[np.isinf(costs)] = -1
      maxCosts = np.max(costs)
      costsNormalized = costs/maxCosts if maxCosts != 0. else costs
            
      cnt = 0
      for trajectory,cost in self._trajectorySet:
        color = pl.cm.summer(costsNormalized[cnt]) if costsNormalized[cnt] >= 0. else 'r'
        ax.plot([s.state.x for s in trajectory.samples], [s.state.y for s in trajectory.samples], 'y', label='trajectory set', color=color)
        cnt+=1
    
    # plot dynamic objects
    for prediction in self._dynamicObjectPredictions:
      trajectory = prediction.getTrajectory()
      ax.plot([item.x for item in trajectory], [item.y for item in trajectory], 'y')
      if len(trajectory) > 0:
        ax.plot(trajectory[0].x, trajectory[0].y, 'oy')
      if prediction.hasBbox():
        bbox = prediction.getBbox()

    # plot solution trajectory
    if not solutionTrajectory is None:
      ax.plot([s.state.x for s in solutionTrajectory.samples], [s.state.y for s in solutionTrajectory.samples], 'b', label='solution', linewidth=3)
    

    pl.axis('equal')
    ax.grid('on')
    ax.set_xlabel('x [m]')
    ax.set_ylabel('y [m]')
    pl.show(block=False)
    fig.canvas.draw()
    
  def getSimulationTime(self, unit='s'):
    ''' Retrieves the current simulation time from V-rep
    '''
    tMilliSec = vrep.simxGetLastCmdTime(self._clientId)
    if unit == 's':
      return self.vrepStampToSec(tMilliSec)
    elif unit == 'us':
      return self.vrepStampToMicrosec(tMilliSec)
    elif unit == 'ms':
      return tMilliSec
    else:
      raise RuntimeError("Invalid unit specifier "+unit)

  @staticmethod
  def getControls(trajectory, stampSec):
    ''' Extracts vehicle controls at time 'stampSec' from a time-indexed trajectory
    '''
    dt = abs(np.array( [s.state.time for s in trajectory.samples] ) - stampSec)
    minIdx = np.argmin(dt)
    return trajectory.samples[int(minIdx)].input
  
  def sendControls(self, controls):
    ''' Sends vehicle controls to V-Rep via named signal
    '''
    desiredSteeringWheelAngleRad = controls.steerAngleSingleTrack
    desiredAccelerationMeterPerSecond2 = controls.acceleration
    vrep.simxSetFloatSignal(self._clientId,'Car_SteeringWheelAngle_Desired'+self._robotId,desiredSteeringWheelAngleRad,vrep.simx_opmode_oneshot);
    vrep.simxSetFloatSignal(self._clientId,'Car_Acceleration_Desired'+self._robotId,desiredAccelerationMeterPerSecond2,vrep.simx_opmode_oneshot);
    
  @staticmethod
  def stringBufferToRawBytes(stringBuffer):
    ''' Converts string to raw bytes
    '''
    return (ctypes.c_ubyte * len(stringBuffer)).from_buffer_copy(stringBuffer)   
    
  def sendTrajectory(self,trajectory):
    ''' Sends a complete trajectory to V-Rep. In V-Rep we will extract the correct command based on the timestamps.
    '''
    packedData = vrep.simxPackFloats(np.array( [[s.state.time for s in trajectory.samples], [s.input.steerAngleSingleTrack for s in trajectory.samples], [s.input.acceleration for s in trajectory.samples] ]  ).flatten())
    rawBytes = self.stringBufferToRawBytes(packedData)
    vrep.simxSetStringSignal(self._clientId,'Car_Trajectory_Desired'+self._robotId,rawBytes,vrep.simx_opmode_oneshot)
    
  def generateDynamicObjectPredictions(self,dynamicObjects, useAngularRate=True):
    ''' Generates a sequence of poses for each dynamic object based on a constant velocity assumption
    '''
    dynamicObjectPredictions = []
    
    for d in dynamicObjects:
      arr = np.array(simxUnpackFloats(d))
      if not arr.size == 16:
        raise RuntimeError("Inconsistent size for dynamic object data returned from V-Rep, expected 16 values, got {0}".format(arr.size))
      position_vrep,euler_vrep,linearVelocity_vrep,angularVelocity_vrep,size,stamp = np.split(arr, [3,6,9,12,15])
      stamp = stamp.squeeze()
      bbox = lpp.BBox(size[0], size[1])
      radius = hypot(size[0]/2., size[1]/2.)
      
      # transform to map frame
      T_map_vrep = self._tfServer.getTransform(self.getSimulationTime('us'), 'vrepFrame', 'mapFrame')
      position_map,euler_map = self._tfServer.transformPose(position_vrep, euler_vrep, T_map_vrep)
      linearVelocity_map = self._tfServer.transformLinearVelocity(linearVelocity_vrep,T_map_vrep)
      angularVelocity_map = self._tfServer.transformLinearVelocity(angularVelocity_vrep,T_map_vrep)
      
      yaw_rate = angularVelocity_map[2]
      yaw = euler_map[2]
      vel_body = linearVelocity_map[0]*cos(yaw) + linearVelocity_map[1]*sin(yaw)
      pos = position_map[0:2]
      
      # now generate the prediction (constant velocity)
      prediction = []
      dt = self._plannerParameters.simulation.simTimeEdgeSec/100
      if useAngularRate:
        for i in range(100):
          yaw += dt*yaw_rate
          vel = vel_body*np.array([cos(yaw), sin(yaw)])
          pos += vel*dt
          state = lpp.DynamicObjectStateStamped(pos[0],
            pos[1],
            yaw,
            hypot(linearVelocity_map[0],linearVelocity_map[1]),
            stamp+i*dt)
          prediction.append(state)
      else:
        for i in range(100):
          pos += linearVelocity_map[0:2]*dt
          state = lpp.DynamicObjectStateStamped(pos[0],
            pos[1],
            yaw,
            hypot(linearVelocity_map[0],linearVelocity_map[1]),
            stamp+i*dt)
          prediction.append(state)
        
      dynamicObjectPrediction = lpp.DynamicObstaclePrediction(prediction, radius)
      dynamicObjectPrediction.setBbox(bbox)
      dynamicObjectPredictions.append(dynamicObjectPrediction)
      
    return dynamicObjectPredictions
    
  def getPathDistanceMap(self):
    ''' Returns the path distance map
    '''
    return self._localPlanner.getPathDistanceMap()
    
  def getObstacleDistanceMap(self):
    ''' Returns the obstacle distance map
    '''
    return self._localPlanner.getObstacleDistanceMap()
    