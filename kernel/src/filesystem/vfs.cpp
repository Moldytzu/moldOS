#include <filesystem/vfs.h>

FileDescriptor::FileDescriptor() {}

void VFSAdd(FileDescriptor toAdd) {
    fastmemcpy((void*)VFSDescriptors[VFSLastDescriptor]->path+1,(void*)toAdd.path,367);
    VFSDescriptors[VFSLastDescriptor]->path[0] = '/';
    VFSDescriptors[VFSLastDescriptor++]->source = toAdd.source;
}

void VFSRemove(const char* path) {

}

void VFSInit()
{
    VFSDescriptors = (FileDescriptor**)malloc(sizeof(FileDescriptor*)*VFS_MAX_DESCRIPTORS);
    for(int i = 0; i<VFS_MAX_DESCRIPTORS; i++)
    {
        VFSDescriptors[i] = (FileDescriptor*)malloc(sizeof(FileDescriptor));
        memset((void*)VFSDescriptors[i],0,sizeof(FileDescriptor));
    }
}

FileDescriptor* VFSOpenFile(const char* path)
{
    for(int i = 0;i<VFS_MAX_DESCRIPTORS;i++)
    {
        if(memcmp(VFSDescriptors[i]->path,path,strlen(path))==0)
            return VFSDescriptors[i];
    }
    return NULL;
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