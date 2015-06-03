#include "mex.h"
#include <windows.h>
#include "shareMemory.cpp"
#include <string.h>

/** Description: 
[double result] = simExtMatlabSM(string Name, string data) : writes shared memory
[double result,string data] = simExtMatlabSM(string Name, double Size) : reads shared memory
[double result,string dataToRead] = simExtMatlabSM(string Name, string dataToWrite) : reads and then write shared memory. 
**/
void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{

   double *ret;
   if(nlhs==1 && nrhs==2 && mxIsChar(prhs[0]) && mxIsChar(prhs[1])) { 
//[double result] = simExtMatlabSM(string Name, string data) : writes shared memory

       char *name = mxArrayToString(prhs[0]);
       char *dataWrite = mxArrayToString(prhs[1]);
       size_t size=(mxGetM(prhs[1])*mxGetN(prhs[1]))+1;
       
       plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL );
       ret    = mxGetPr(plhs[0]);
       *ret=-1;
       
       shareMemory sMemory(name,size);
       if(sMemory.openShareMemory()==true){
           if(sMemory.writeShareMemory(dataWrite)==true)
               *ret=1;
       }
       if(sMemory.closeShareMemory()==false)
           *ret=0; //memory was written, but could not close
       mxFree(name);
       mxFree(dataWrite);
       return;
   }
   if(nlhs==2 && nrhs==2 && mxIsChar(prhs[0]) && mxIsInt32(prhs[1])) { 
    //[double result,char data] = simExtMatlabSM(string Name, double Size) : reads shared memory

       char *name = mxArrayToString(prhs[0]);
       int *size=(int*) mxGetData(prhs[1]);
       
       plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL );
       ret     = mxGetPr(plhs[0]);
       *ret=-1;
       plhs[1] = mxCreateNumericMatrix(1,*size,mxUINT8_CLASS,mxREAL );
       char *dataRead= (char*)mxGetData(plhs[1]);
       
       shareMemory sMemory(name,*size);
       if(sMemory.openShareMemory()==true){
           if(sMemory.readShareMemory((char*)mxGetData(plhs[1]))==true){
               *ret=1;
           }
       }
       if(sMemory.closeShareMemory()==false)
           *ret=0; //memory was written, but could not close*/
       mxFree(name);
       return;
   }
   return; 
}
