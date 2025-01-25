#define _CRT_SECURE_NO_WARNINGS
#include "fs.h"
#include <stdlib.h>

int main(int argc, char** argv) {
	if (!fopen("fs.bin", "r"))
		BdfsClient_Create("fs.bin");

	BdfsClient_OpenFile("fs.bin");
	unsigned long RootDir = BdfsClient_GetDirHandleByPath("/");
	

	
	/*
	unsigned long NewFile = BdfsClient_CreateFile(RootDir, "test.bin");

	char FileBuffer[] = "This is a test file.";

	BdfsClient_UpdateFile(NewFile, FileBuffer, sizeof(FileBuffer));
	char* Buffer = BdfsClient_ReadFile(NewFile);
	printf("File: %s\n", Buffer);
	free(Buffer);*/

	return;
}