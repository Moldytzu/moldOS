#include <filesystem/vfs.h>

FileDescriptor* VFSOpenFile(const char* path)
{
    FileDescriptor* descriptor = (FileDescriptor*)GlobalAllocator.RequestPage();
    memset((void*)descriptor,0,4096);
    asmmemcpy((void*)descriptor->path,(void*)path,strlen(path)+1);
    descriptor->source = VFSSource;
    return descriptor;
}

void* VFSReadFile(FileDescriptor* file)
{
    if(file == NULL) return NULL;
    switch (file->source)
    {
    case VFS_SOURCE_RAMFS:
        return LLFSVFSHelperReader(file->path);
        break;

    default:
        return NULL; //TODO: call driver
        break;
    }
}