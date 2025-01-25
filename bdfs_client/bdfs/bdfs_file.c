#include "../fs.h"
#include <stdlib.h>

unsigned long BdfsClient_GetFileSize(unsigned long FileHandle) {
	FS_FILE ThisFile = { 0 };
	BdfsClient_RawRead(&ThisFile, FileHandle, sizeof(FS_FILE));
	return ThisFile.DataSize;
}

void* BdfsClient_ReadFile(unsigned long FileHandle) {
	FS_FILE ThisFile = { 0 };
	BdfsClient_RawRead(&ThisFile, FileHandle, sizeof(FS_FILE));

	void* Buffer = malloc(ThisFile.DataSize);
	BdfsClient_RawRead(Buffer, ThisFile.DataLocation, ThisFile.DataSize);

	return Buffer;
}

void BdfsClient_UpdateFile(unsigned long FileHandle, void* Data, unsigned long NewSize) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FS_HEAD));

	FS_FILE ThisFile = { 0 };
	BdfsClient_RawRead(&ThisFile, FileHandle, sizeof(FS_FILE));

	if (ThisFile.DataLocation == 0) {
		ThisFile.DataLocation = FsHead.FsHigh;
		FsHead.FsHigh += NewSize;
		ThisFile.DataSize = NewSize;
		BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
		BdfsClient_RawWrite(&ThisFile, FileHandle, sizeof(FS_FILE));
	}

	if (NewSize > ThisFile.DataSize && ThisFile.DataSize != 0) {
		// move to a new chunk, create a gap at the old file
		FS_GAP ThisGap = { 0 };
		unsigned long Me2 = FsHead.FirstGap;
		BdfsClient_RawRead(&ThisGap, FsHead.FirstGap, sizeof(FS_GAP));
		do {
			if (!ThisGap.Next) {
				ThisGap.Next = FsHead.FsHigh;
				FsHead.FsHigh += sizeof(FS_GAP);

				FS_GAP NewGap0 = { 0 };
				NewGap0.GapLocation = ThisFile.DataLocation;
				NewGap0.GapSize = ThisFile.DataSize;
				NewGap0.Next = FsHead.FsHigh;

				FsHead.FsHigh += sizeof(FS_GAP);
				BdfsClient_RawWrite(&NewGap0, ThisGap.Next, sizeof(FS_GAP));
				BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
			}

			Me2 = ThisGap.Next;
			BdfsClient_RawRead(&ThisGap, ThisGap.Next, sizeof(FS_GAP));
		} while (ThisGap.Next);

		// update the file information
		ThisFile.DataLocation = FsHead.FsHigh;
		ThisFile.DataSize = NewSize;
		FsHead.FsHigh += NewSize;
		BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
		BdfsClient_RawWrite(&ThisFile, FileHandle, sizeof(FS_FILE));

	}
	else if (NewSize < ThisFile.DataSize) {
		// create a gap with the difference
		FS_GAP ThisGap = { 0 };
		unsigned long Me2 = FsHead.FirstGap;
		BdfsClient_RawRead(&ThisGap, FsHead.FirstGap, sizeof(FS_GAP));
		do {
			if (!ThisGap.Next) {
				ThisGap.Next = FsHead.FsHigh;
				FsHead.FsHigh += sizeof(FS_GAP);

				FS_GAP NewGap0 = { 0 };
				NewGap0.GapLocation = ThisFile.DataLocation + NewSize;
				NewGap0.GapSize = ThisFile.DataSize - NewSize;
				NewGap0.Next = FsHead.FsHigh;

				FsHead.FsHigh += sizeof(FS_GAP);
				BdfsClient_RawWrite(&NewGap0, ThisGap.Next, sizeof(FS_GAP));
				BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
			}

			Me2 = ThisGap.Next;
			BdfsClient_RawRead(&ThisGap, ThisGap.Next, sizeof(FS_GAP));
		} while (ThisGap.Next);

		ThisFile.DataSize = NewSize;
		BdfsClient_RawWrite(&ThisFile, FileHandle, sizeof(FS_FILE));
	}

	BdfsClient_RawWrite(Data, ThisFile.DataLocation, NewSize);
	return;
}

void BdfsClient_DeleteFile(unsigned long FileHandle) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FS_HEAD));

	FS_FILE File = { 0 };
	BdfsClient_RawRead(&File, FileHandle, sizeof(FS_FILE));

	if (!FsHead.FirstGap) {
		FsHead.FirstGap = FsHead.FsHigh;
		FsHead.FsHigh += sizeof(FS_GAP);

		FS_GAP NewGap = { 0 };
		NewGap.GapLocation = FileHandle;
		NewGap.GapSize = sizeof(FS_FILE);
		NewGap.Next = FsHead.FsHigh;
		FsHead.FsHigh += sizeof(FS_GAP);

		FS_GAP NewGap2 = { 0 };
		NewGap2.GapLocation = File.DataLocation;
		NewGap2.GapSize = File.DataSize;

		BdfsClient_RawWrite(&NewGap, FsHead.FirstGap, sizeof(FS_GAP));
		BdfsClient_RawWrite(&NewGap2, NewGap.Next, sizeof(FS_GAP));
		BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
	}
	else {
		FS_GAP ThisGap = { 0 };
		unsigned long Me2 = FsHead.FirstGap;
		BdfsClient_RawRead(&ThisGap, FsHead.FirstGap, sizeof(FS_GAP));
		do {
			if (!ThisGap.Next) {
				ThisGap.Next = FsHead.FsHigh;
				FsHead.FsHigh += sizeof(FS_GAP);

				FS_GAP NewGap0 = { 0 };
				NewGap0.GapLocation = FileHandle;
				NewGap0.GapSize = sizeof(FS_FILE);
				NewGap0.Next = FsHead.FsHigh;

				FS_GAP NewGap1 = { 0 };
				NewGap1.GapLocation = File.DataLocation;
				NewGap1.GapSize = File.DataSize;
				NewGap1.Next = 0;

				FsHead.FsHigh += sizeof(FS_GAP);

				BdfsClient_RawWrite(&NewGap0, ThisGap.Next, sizeof(FS_GAP));
				BdfsClient_RawWrite(&NewGap1, NewGap0.Next, sizeof(FS_GAP));
				BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
			}

			BdfsClient_RawRead(&ThisGap, ThisGap.Next, sizeof(FS_GAP));
		} while (ThisGap.Next);
	}

	return;
}