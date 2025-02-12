#define _CRT_SECURE_NO_WARNINGS
#include "../fs.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#pragma warning(disable: 6011)

unsigned long BdfsClient_GetDirHandleByPath(char* Path) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FS_HEAD));

	if (!strcmp(Path, "/"))
		return FsHead.FirstDirectory;

	char* Token = strtok(Path, "/");
	unsigned long CurrentDirId = 0;
	unsigned long CurrentDirLoc = FsHead.FirstDirectory;
	unsigned long TokenHash;

	while (Token) {
		TokenHash = BdfsClient_HashStr(Token);
		FS_DIRECTORY CurrentDir = { 0 };
		unsigned long DirFound = 0;
		unsigned long DirId = CurrentDirLoc;

		while (DirId) {
			BdfsClient_RawRead(&CurrentDir, DirId, sizeof(CurrentDir));
			if (CurrentDir.DirNameHash == TokenHash) {
				DirFound = 1;
				break;
			}

			DirId = CurrentDir.Next;
		}

		if (!DirFound) {
			continue;
		}

		CurrentDirLoc = DirId;
		Token = strtok(NULL, "/");
	}

	return CurrentDirLoc;
}

unsigned long BdfsClient_GetDirEntryCount(unsigned long DirHandle) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	FS_ENTRY EntryHdr = { 0 };
	BdfsClient_RawRead(&EntryHdr, FsHead.FirstEntry, sizeof(FS_ENTRY));
	unsigned long Count = 0;
	do {
		for (int i = 0; i < 256; i++) {
			if (EntryHdr.Entries[i].FileParentDirLoc == DirHandle)
				Count++;
		}

		BdfsClient_RawRead(&EntryHdr, EntryHdr.Next, sizeof(FS_ENTRY));
	} while (EntryHdr.Next);

	return Count;
}

char* BdfsClient_GetDirEntryName(unsigned long DirHandle, unsigned long Iterator) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	FS_ENTRY EntryHdr = { 0 };
	BdfsClient_RawRead(&EntryHdr, FsHead.FirstEntry, sizeof(FS_ENTRY));
	unsigned long Count = 0;
	do {
		for (int i = 0; i < 256; i++) {
			if (EntryHdr.Entries[i].FileParentDirLoc == DirHandle) {	
				if (Count == Iterator) {
					FS_FILE File = { 0 };
					BdfsClient_RawRead(&File, EntryHdr.Entries[i].FileMetaLocation, sizeof(FS_FILE));
					return _strdup(File.FileName);
				}

				Count++;
			}
		}

		BdfsClient_RawRead(&EntryHdr, EntryHdr.Next, sizeof(FS_ENTRY));
	} while (EntryHdr.Next);

	return Count;
}

unsigned long BdfsClient_GetFileHandleByIterator(unsigned long DirHandle, unsigned long Iterator) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	FS_ENTRY EntryHdr = { 0 };
	BdfsClient_RawRead(&EntryHdr, FsHead.FirstEntry, sizeof(FS_ENTRY));
	unsigned long Count = 0;
	do {
		for (int i = 0; i < 256; i++) {
			if (EntryHdr.Entries[i].FileParentDirLoc == DirHandle)
				Count++;
			if (Count == Iterator) {
				return EntryHdr.Entries[i].FileMetaLocation;
			}
		}

		BdfsClient_RawRead(&EntryHdr, EntryHdr.Next, sizeof(FS_ENTRY));
	} while (EntryHdr.Next);

	return FS_INVALID;
}

unsigned long BdfsClient_GetFileHandleByPath(char* Path) {
	char* FileName = strstr(Path, "/");
	FileName++;
	char* File = strdup(FileName);

	char* _Path = strdup(Path + 1);
	strstr(_Path, "/")[0] = 0x00;

	unsigned long DirLoc = BdfsClient_GetDirHandleByPath(_Path);
	free(_Path);

	unsigned long FileNameHash = BdfsClient_HashStr(File);
	free(File);

	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	FS_ENTRY EntryHdr = { 0 };
	BdfsClient_RawRead(&EntryHdr, FsHead.FirstEntry, sizeof(FS_ENTRY));
	unsigned long Count = 0;
	do {
		for (int i = 0; i < 256; i++) {
			if (EntryHdr.Entries[i].FileParentDirLoc == DirLoc)
				Count++;
			if (EntryHdr.Entries[i].FileNameHash == FileNameHash) {
				return EntryHdr.Entries[i].FileMetaLocation;
			}
		}

		BdfsClient_RawRead(&EntryHdr, EntryHdr.Next, sizeof(FS_ENTRY));
	} while (EntryHdr.Next);

	return FS_INVALID;
}

unsigned long BdfsClient_CreateFile(unsigned long DirHandle, char* FileName) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	unsigned long NameHash = BdfsClient_HashStr(FileName);

	// verify file doesn't already exist
	FS_ENTRY ThisEntry = { 0 };
	BdfsClient_RawRead(&ThisEntry, FsHead.FirstEntry, sizeof(FS_ENTRY));
	do {
		for (int i = 0; i < 256; i++) {
			if (ThisEntry.Entries[i].FileNameHash == NameHash)
				return FS_INVALID;
		}

		BdfsClient_RawRead(&ThisEntry, ThisEntry.Next, sizeof(FS_ENTRY));
	} while (ThisEntry.Next);

	if (FsHead.FirstEntry == 0) {
		FS_ENTRY NewEntry = { 0 };
		NewEntry.Entries[0].FileMetaLocation = FsHead.FsHigh;
		NewEntry.Entries[0].FileParentDirLoc = sizeof(FS_HEAD); // first dir is always at FS_HEAD + 0
		NewEntry.Entries[0].FileNameHash = NameHash;

		FS_FILE NewFile = { 0 };
		NewFile.Created = time(NULL);
		strcpy(NewFile.FileName, FileName);
		NewFile.FileNameHash = NameHash;

		BdfsClient_RawWrite(&NewFile, FsHead.FsHigh, sizeof(FS_FILE));
		FsHead.FsHigh += sizeof(FS_FILE);
		BdfsClient_RawWrite(&NewEntry, FsHead.FsHigh, sizeof(FS_ENTRY));
		FsHead.FirstEntry = FsHead.FsHigh;
		FsHead.FsHigh += sizeof(FS_ENTRY);
		BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));

		return NewEntry.Entries[0].FileMetaLocation;
	}

	FS_ENTRY EntryHdr = { 0 };
	BdfsClient_RawRead(&EntryHdr, FsHead.FirstEntry, sizeof(FS_ENTRY));
	unsigned long Me = FsHead.FirstEntry;
	unsigned long Count = 0;
	unsigned long FileLoc;
	do {
		for (int i = 0; i < 256; i++) {
			if (EntryHdr.Entries[i].FileParentDirLoc == DirHandle)
				Count++;
			if (EntryHdr.Entries[i].FileMetaLocation == 0) {
				// first free file
				// @TODO search gaps to fill with metadata
				
				EntryHdr.Entries[i].FileMetaLocation = FsHead.FsHigh;
				FileLoc = FsHead.FsHigh;
				EntryHdr.Entries[i].FileNameHash = NameHash;
				EntryHdr.Entries[i].FileParentDirLoc = DirHandle;
	 
				BdfsClient_RawWrite(&EntryHdr, Me, sizeof(EntryHdr));

				FS_FILE NewFile = { 0 };
				NewFile.Created = time(NULL);
				strcpy(NewFile.FileName, FileName);
				NewFile.FileNameHash = NameHash;
				
				BdfsClient_RawWrite(&NewFile, FsHead.FsHigh, sizeof(NewFile));
				FsHead.FsHigh += sizeof(NewFile);
				BdfsClient_RawWrite(&FsHead, 0, sizeof(FsHead));

				return FileLoc;
			}
		}

		if (EntryHdr.Next == 0) {
			EntryHdr.Next = FsHead.FsHigh;
			FsHead.FsHigh += sizeof(FS_ENTRY);
			
			FS_ENTRY NewEntry = { 0 };
			NewEntry.Entries[0].FileMetaLocation = FsHead.FsHigh;
			FileLoc = FsHead.FsHigh;
			NewEntry.Entries[0].FileParentDirLoc = sizeof(FS_HEAD); // first dir is always at FS_HEAD + 0
			NewEntry.Entries[0].FileNameHash = NameHash;

			FS_FILE NewFile = { 0 };
			NewFile.Created = time(NULL);
			strcpy(NewFile.FileName, FileName);
			NewFile.FileNameHash = NameHash;

			BdfsClient_RawWrite(&EntryHdr, Me, sizeof(FS_ENTRY));
			BdfsClient_RawWrite(&NewFile, FsHead.FsHigh, sizeof(FS_FILE));
			FsHead.FsHigh += sizeof(FS_FILE);
			BdfsClient_RawWrite(&NewEntry, FsHead.FsHigh, sizeof(FS_ENTRY));
			FsHead.FsHigh += sizeof(FS_ENTRY);
			BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
		}

		BdfsClient_RawRead(&EntryHdr, EntryHdr.Next, sizeof(FS_ENTRY));
		Me = EntryHdr.Next;
	} while (EntryHdr.Next);

	return FileLoc;
}

unsigned long BdfsClient_CreateDir(unsigned long ParentHnd, char* Name) {
	FS_HEAD FsHead = { 0 };
	BdfsClient_RawRead(&FsHead, 0, sizeof(FsHead));

	unsigned long FirstDir = FsHead.FirstDirectory;

	FS_DIRECTORY Directory = { 0 };
	BdfsClient_RawRead(&Directory, FirstDir, sizeof(FS_DIRECTORY));
	unsigned long Me = FirstDir;
	unsigned long DirHashName = BdfsClient_HashStr(Name);
	do {
		if (Directory.DirNameHash == DirHashName)
			return FS_INVALID;

		if (Directory.Next == 0) {
			Directory.Next = FsHead.FsHigh;
			FsHead.FsHigh += sizeof(FS_DIRECTORY);
			BdfsClient_RawWrite(&Directory, Me, sizeof(FS_DIRECTORY));
			BdfsClient_RawWrite(&FsHead, 0, sizeof(FS_HEAD));
		
			FS_DIRECTORY NewDir = { 0 };
			NewDir.Created = time(NULL);
			strcpy(NewDir.DirectoryName, Name);
			NewDir.DirNameHash = BdfsClient_HashStr(Name);
			NewDir.ParentLoc = ParentHnd;
			BdfsClient_RawWrite(&NewDir, Directory.Next, sizeof(FS_DIRECTORY));
			return Directory.Next;
		}

		Me = Directory.Next;
		BdfsClient_RawRead(&Directory, Directory.Next, sizeof(FS_DIRECTORY));
	} while (Directory.Next);
}