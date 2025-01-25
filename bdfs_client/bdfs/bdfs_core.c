#include "../fs.h"

void BdfsClient_Init(void);
void BdfsClient_Shutdown(void);

// MSVC implementation
unsigned long BdfsClient_OpenFile(const char* File);
void BdfsClient_CloseFile(unsigned long Handle);
