#include "mex.h"
#include "Cevent.cpp"
#include <windows.h>
#include <string.h>

void mexFunction(int nlhs, mxArray *plhs[],int nrhs, const mxArray *prhs[])
{
   double ret=-1;
   if( nrhs==1 && mxIsChar(prhs[0])) { 
    //[double result] = matlabSetEvent(string Name) : sets event

       char *name = mxArrayToString(prhs[0]);
       Cevent event(name);
       if(event.create()==true){
           if(event.set()==true)
               ret=1;
       }
       mxFree(name);
   }
   
   if(nlhs==1){
       plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL );
       *mxGetPr(plhs[0])=ret;
   }
   return; 
}
