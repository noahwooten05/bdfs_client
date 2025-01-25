#include "../fs.h"

unsigned long BdfsClient_GetDirHandleByPath(char* Path);
unsigned long BdfsClient_GetDirEntryCount(unsigned long DirHandle);
char* BdfsClient_GetDirEntryName(unsigned long DirHandle, unsigned long Iterator);
unsigned long BdfsClient_GetFileHandleByIterator(unsigned long DirHandle, unsigned long Iterator);
unsigned long BdfsClient_GetFileHandleByPath(char* Path);
unsigned long BdfsClient_CreateFile(unsigned long DirHandle, char* FileName);
