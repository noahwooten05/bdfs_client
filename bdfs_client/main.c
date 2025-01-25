#define _CRT_SECURE_NO_WARNINGS
#include "fs.h"
#include <stdlib.h>

int main(int argc, char** argv) {
	if (!fopen("fs.bin", "r"))
		BdfsClient_Create("fs.bin");

	BdfsClient_OpenFile("fs.bin");
	unsigned long RootDir = BdfsClient_GetDirHandleByPath("/");
	
	unsigned long NewFile;
	char FileBuffer[] = "This is a test file.";

	NewFile = BdfsClient_CreateFile(RootDir, "test0.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test1.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test2.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test3.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	NewFile = BdfsClient_CreateFile(RootDir, "test4.bin");
	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));

	return;
}