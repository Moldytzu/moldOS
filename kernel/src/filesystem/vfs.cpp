#include <filesystem/vfs.h>

void VFSAdd(FileDescriptor toAdd)
{
    for(int i = 0; i<VFS_MAX_DESCRIPTORS; i++)
    {
        if(VFSDescriptors[i]->path[0] == 0) //free descriptor
        {
            VFSTotalEntries++;
            fastmemcpy((void*)VFSDescriptors[i]->path+1,(void*)toAdd.path,367);
            VFSDescriptors[i]->path[0] = '/';
            VFSDescriptors[i]->source = toAdd.source;
            return;
        }
    }
}

void VFSRemove(const char* path)
{
    for(int i = 0; i<VFS_MAX_DESCRIPTORS; i++)
    {
        if(memcmp(VFSDescriptors[i]->path,path,strlen(path))==0)
        {
            VFSTotalEntries--;
            memset((void*)VFSDescriptors[i]->path,0,368);
            switch (VFSDescriptors[i]->source)
            {
            case VFS_SOURCE_RAMFS:
                //can't remove file from ramfs
                break;

            default:
                //call driver to delete file from the filesystem
                break;
            }

            return;
        }
    }
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
    for(int i = 0; i<VFS_MAX_DESCRIPTORS; i++)
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

bool VFSExistsFileAt(const char* path)
{
    for(int i = 0; i<VFS_MAX_DESCRIPTORS; i++)
    {
        if(memcmp(VFSDescriptors[i]->path,path,strlen(path))==0)
            return true;
    }
    return false;
}