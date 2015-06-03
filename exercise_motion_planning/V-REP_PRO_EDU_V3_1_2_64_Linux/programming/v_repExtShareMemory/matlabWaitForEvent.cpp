#include "mex.h"
#include <windows.h>
#include "Cevent.cpp"
#include <string.h>

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
   double ret;
   DWORD time;
   //[double result] = matlabWaitForEvent(string Name, double time) : reads shared memory
   if(nrhs>=1 && mxIsChar(prhs[0])){
       if(nrhs==2 && mxIsDouble(prhs[1])){ 
           double *timeIn=mxGetPr(prhs[1]);
           time=(DWORD)*timeIn;
       }
       else
           time=INFINITE;
   
       char *name = mxArrayToString(prhs[0]);

       Cevent event(name);
       if(event.create()==true){
           if(event.waitForEvent((DWORD)time)==true)
               ret=1;
           else
               ret=0;
       }
       
       mxFree(name);
   }
   if(nlhs==1){
       plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL );
       *mxGetPr(plhs[0])=ret;
   }
   return; 
}
