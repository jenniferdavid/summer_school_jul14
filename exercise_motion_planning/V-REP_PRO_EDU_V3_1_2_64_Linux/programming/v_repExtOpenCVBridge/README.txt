v-rep OpenCVBridge v1.1
by Marco Bellaccini - marco.bellaccini[at!]gmail.com
This v-rep plugin makes it possible to quickly perform OpenCV2 filtering and elaboration on images coming from v-rep vision sensors/cameras.
The plugin acts as a v-rep image processing filter.

THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY, UNDER
THE TERMS OF THE GNU LESSER GENERAL PUBLIC LICENSE, VERSION 3

In order to use the v-rep OpenCVBridge you should:

IF YOU'RE USING LINUX:

0)of course, make sure you have Opencv2 libraries installed

1)update v-rep-related files by copying your "common" and "include" directories from the "programming" folder (that one located in your v-rep main program folder) to the v_repExtOpenCVBridge directory - so substituting the two folders provided with this plugin

2)edit the "myOpenCVElab.cpp" file as to perform your custom elaboration/filtering on the image

3)compile the plugin and your custom code by running "make"

4)copy the resulting library ("libv_repExtOpenCVBridge.so" in Linux) to your v-rep main program folder

5)add the "OpenCV elaboration" filter in the filtering chain through the v-rep vision sensor "Image processing and Triggering" dialog

For further details, after performing steps 1-4, run the "bubbleRobOpenCVTest.ttt" example scene coming in-bundle with this plugin.
NOTE: in order to correctly run the example, you'll probably have to remove the "OpenCV elaboration" filter from the filtering chain (in the v-rep vision sensor "Image processing and Triggering" dialog) and re-insert it

IF YOU'RE USING WINDOWS:
0)of course, make sure you have Opencv2 libraries installed

1)update v-rep-related files by copying your "common" and "include" directories from the "programming" folder (that one located in your v-rep main program folder) to the v_repExtOpenCVBridge directory - so substituting the two folders provided with this plugin

2)edit the "myOpenCVElab.cpp" file as to perform your custom elaboration/filtering on the image

3)open the "v_repExtOpenCVBridge.vcxproj" file with MS Visual C++/MS Visual C++ Express (version 2010 should be fine) and compile the project
IMPORTANT: here I assume that you have already correctely set up your IDE with OpenCV and that you have OpenCV 2.4.7 installed in C:\opencv.
So maybe you have to edit some settings in the vcxproj project file to fit your configuration. Refer to the OpenCV documentation for details.

4)copy the resulting library ("v_repExtOpenCVBridge.dll" in Windows) to your v-rep main program folder

5)add the "OpenCV elaboration" filter in the filtering chain through the v-rep vision sensor "Image processing and Triggering" dialog

For further details, after performing steps 1-4, run the "bubbleRobOpenCVTest.ttt" example scene coming in-bundle with this plugin.
NOTE: in order to correctly run the example, you'll probably have to remove the "OpenCV elaboration" filter from the filtering chain (in the v-rep vision sensor "Image processing and Triggering" dialog) and re-insert it


