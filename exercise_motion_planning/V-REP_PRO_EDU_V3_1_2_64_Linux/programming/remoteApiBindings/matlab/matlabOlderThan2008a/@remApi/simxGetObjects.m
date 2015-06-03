function [rtn objectHandles] = simxGetObjects(obj,clientID,objectType,operationMode)
			objectType_ = int32(objectType);
			objectHandles_ = libpointer('int32PtrPtr');
			objectCount = libpointer('int32Ptr',int32(0));
			operationMode_ = int32(operationMode);

			[rtn objectCount objectHandles_]  = calllib(obj.libName,'simxGetObjects',clientID,objectType_,objectCount,objectHandles_,operationMode_);
			
			if (rtn==0)
				if(objectCount > 0 )  
					objectHandles_.setdatatype('int32Ptr',1,objectCount);
					objectHandles = objectHandles_.value;
				else
					objectHandles = [];
				end
			else
				objectHandles = [];
			end
		end