#pragma once
#include <stdio.h>
#define FS_MAGIC 'BDFS'
#define FS_INVALID 0xFFFFFFFF

typedef struct _FS_HEAD {
	unsigned long Magic;

	unsigned long FirstDirectory;
	unsigned long FirstFile;
	unsigned long FirstEntry;
	unsigned long FirstGap;

	unsigned long FsHigh;
}FS_HEAD, * PFS_HEAD;

typedef struct _FS_DIRECTORY {
	unsigned long ParentLoc;
	char DirectoryName[64];
	unsigned long DirNameHash;
	unsigned long Created, Modified, 
		Accessed;

	unsigned long Next;
}FS_DIRECTORY, * PFS_DIRECTORY;

typedef struct _FS_FILE {
	unsigned long FileNameHash;
	char FileName[64];
	unsigned long DataLocation;
	unsigned long DataSize;
	unsigned long Created, Modified, 
		Accessed;
}FS_FILE, * PFS_FILE;

typedef struct _FS_ENTRY_SUB {
	unsigned long FileParentDirLoc;
	unsigned long FileNameHash;
	unsigned long FileMetaLocation;
}FS_ENTRY_SUB, * PFS_ENTRY_SUB;

typedef struct _FS_ENTRY {
	FS_ENTRY_SUB Entries[256];
	unsigned long Next;
}FS_ENTRY, * PFS_ENTRY;

typedef struct _FS_GAP {
	unsigned long GapLocation;
	unsigned long GapSize;
	unsigned long Next;
	unsigned char Used;
}FS_GAP, * PFS_GAP;

void BdfsClient_Create(const char* File);
void BdfsClient_OpenFile(const char* File);
void BdfsClient_CloseFile(void);
void BdfsClient_RawRead(void* Buffer, unsigned long Location, unsigned long Size);
void BdfsClient_RawWrite(void* Buffer, unsigned long Location, unsigned long Size);
unsigned long BdfsClient_HashStr(char* Str);

unsigned long BdfsClient_GetDirHandleByPath(char* Path);
unsigned long BdfsClient_GetDirEntryCount(unsigned long DirHandle);
char* BdfsClient_GetDirEntryName(unsigned long DirHandle, unsigned long Iterator);
unsigned long BdfsClient_GetFileHandleByIterator(unsigned long DirHandle, unsigned long Iterator);
unsigned long BdfsClient_GetFileHandleByPath(char* Path);
unsigned long BdfsClient_CreateFile(unsigned long DirHandle, char* FileName);
unsigned long BdfsClient_CreateDir(unsigned long ParentHnd, char* Name);

unsigned long BdfsClient_GetFileSize(unsigned long FileHandle);
void* BdfsClient_ReadFile(unsigned long FileHandle);
void BdfsClient_UpdateFile(unsigned long FileHandle, void* Data, unsigned long NewSize);
void BdfsClient_DeleteFile(unsigned long FileHandle);