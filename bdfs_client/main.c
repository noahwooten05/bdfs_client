#define _CRT_SECURE_NO_WARNINGS
#include "fs.h"
#include <stdlib.h>

int main(int argc, char** argv) {
	if (!fopen("fs.bin", "r")) {
		BdfsClient_Create("fs.bin");
		BdfsClient_CreateFile(BdfsClient_GetDirHandleByPath("/"), "$rootfile.bin");
	}

	BdfsClient_OpenFile("fs.bin");
	unsigned long RootDir = BdfsClient_GetDirHandleByPath("/");
	
	unsigned long NewFile;
	char FileBuffer[] = "This is a test file.";

	NewFile = BdfsClient_CreateFile(RootDir, "rootfile0.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test1.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test2.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test3.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test4.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test5.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));

	int FilesCount = BdfsClient_GetDirEntryCount(RootDir);
	for (int i = 0; i < FilesCount; i++) {
		char* Name = BdfsClient_GetDirEntryName(RootDir, i);
		printf("/%s\n", Name);
		free(Name);
	}

	unsigned long NewDir = BdfsClient_CreateDir(RootDir, "dir0");
	NewFile = BdfsClient_CreateFile(NewDir, "test6.bin");

	unsigned long FileHandle = BdfsClient_GetFileHandleByPath("/dir0/test6.bin");
	char* Buffer = BdfsClient_ReadFile(FileHandle);
	printf("File: %s\n", Buffer);
	free(Buffer);

	return;
}