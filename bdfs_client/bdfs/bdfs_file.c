#include "../fs.h"

unsigned long BdfsClient_GetFileSize(unsigned long FileHandle);
void* BdfsClient_ReadFile(unsigned long FileHandle);
void BdfsClient_UpdateFile(unsigned long FileHandle, void* Data, unsigned long NewSize);
void BdfsClient_DeleteFile(unsigned long FileHandle);