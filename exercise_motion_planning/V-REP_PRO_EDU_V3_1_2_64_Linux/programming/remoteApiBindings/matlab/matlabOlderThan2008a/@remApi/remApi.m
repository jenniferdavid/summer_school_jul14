function obj = remApi(libname,hfile)
    obj = struct;
    obj.libName = libname;
    obj.hFile = hfile;
    obj=class(obj,'remApi');
    loadlibrary(obj.libName,obj.hFile);
end