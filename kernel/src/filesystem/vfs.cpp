#include <filesystem/vfs.h>

FileDescriptor* VFSOpenFile(const char* path)
{
    FileDescriptor* descriptor = (FileDescriptor*)GlobalAllocator.RequestPage();
    memset((void*)descriptor,0,4096);
    fastmemcpy((void*)descriptor->path,(void*)path,strlen(path)+1);
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

uint64_t VFSSizeFile(FileDescriptor* file)
{
    if(file == NULL) return 0;
    switch (file->source)
    {
    case VFS_SOURCE_RAMFS:
        return LLFSOpenFile(file->path+1)->FileSize;
        break;

    default:
        return 0; //TODO: call driver
        break;
    }
}