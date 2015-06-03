The files in this directory are courtesy of Gerold Huber:

as my company only has a license for Matlab 2007b, i ran into a problem with the VRep Remote API. Appareantly it's programmed in "new style objec oriented programming" which was introduced in Maltab 2008a. (Information on the differences: https://yagtom.googlecode.com/svn-history/trunk/html/objectOriented.html)

So I translated the API to "old style OOP" and wanted to share it with the community:
https://drive.google.com/file/d/0B5ib6IviiWfHUmxNTGUyVm1wekU/edit?usp=sharing
(Note: I only made it work with 2007b 32bit version, as the command loadlibrary('remoteApi','extApi.h') seems to not work with 2007b 64bit, because the use of the LCC compiler is hardcoded, however this compiler is not supported in the 64bit edition)
For the translation from new style to old style I split up the class remApi as follows: (cause "classded" is not supported):

* loadRemApiConstatns.m defining all the constants
* unloadRemApiConstatns.m deleting all the constants
* folder @remApi with a separate file for each method

In this folder you will also find an example matlab script that interacts with VRep.