% destructor
function delete(obj)
    unloadlibrary(obj.libName);
end