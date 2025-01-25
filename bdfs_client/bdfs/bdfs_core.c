#include "../fs.h"
#include <stdio.h>

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