#define _CRT_SECURE_NO_WARNINGS
#include "../fs.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

FILE* CrtFile;

void BdfsClient_OpenFile(const char* File) {
	CrtFile = fopen(File, "rb+");
	return;
}

void BdfsClient_CloseFile(void) {
	fclose(CrtFile);
	return;
}

void BdfsClient_RawRead(void* Buffer, unsigned long Location, unsigned long Size) {
	fseek(CrtFile, Location, SEEK_SET);
	fread(Buffer, Size, 1, CrtFile);
	return;
}

void BdfsClient_RawWrite(void* Buffer, unsigned long Location, unsigned long Size) {
	fseek(CrtFile, Location, SEEK_SET);
	fwrite(Buffer, Size, 1, CrtFile);
	return;
}

void BdfsClient_Create(const char* File) {
	CrtFile = fopen(File, "wb+");
	
	FS_HEAD FsHead = { 0 };
	FsHead.Magic = FS_MAGIC;
	FsHead.FsHigh = sizeof(FS_HEAD);

	FsHead.FirstDirectory = FsHead.FsHigh;
	FS_DIRECTORY FsDir = { 0 };
	FsDir.Created = time(NULL);
	strcpy(FsDir.DirectoryName, "$ROOT");
	FsDir.DirNameHash = BdfsClient_HashStr("$ROOT");
	FsHead.FsHigh += sizeof(FS_DIRECTORY);

	BdfsClient_RawWrite(&FsHead, 0, sizeof(FsHead));
	BdfsClient_RawWrite(&FsDir, FsHead.FirstDirectory, sizeof(FsDir));

	return;
}

unsigned long BdfsClient_HashStr(char* Str) {
	unsigned long hash = 5381;
	int c;
	while ((c = *Str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}