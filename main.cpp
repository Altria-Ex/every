#include<Windows.h>
#include<iostream>

#define BUF_LEN 4096

using namespace std;

int GetFileSystemName(char* FileSystemName, const char* RootPathName = "D:")
{
	char FileSystemNameBuffer[MAX_PATH];
	int status = GetVolumeInformationA(
		RootPathName,
		NULL,
		0,
		NULL,
		NULL,
		NULL,
		FileSystemNameBuffer,
		MAX_PATH
	);

	if (status != 0)
	{
		strcpy_s(FileSystemName, MAX_PATH, FileSystemNameBuffer);
		return 0;
	}
	else
		return -1;
}

int GetVolumeHandle(HANDLE& handle, const char* RootPathName = "D:")
{
	char filename[MAX_PATH] = "\\\\.\\";
	strcat_s(filename, RootPathName);
	HANDLE h = CreateFileA(
		filename,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_READONLY,
		NULL
	);

	if (h != INVALID_HANDLE_VALUE)
	{
		handle = h;
		return 0;
	}
	else
		return -1;
}

int CreateUSNJournal(HANDLE h)
{
	DWORD br;
	CREATE_USN_JOURNAL_DATA cujd;
	cujd.MaximumSize = 0;
	cujd.AllocationDelta = 0;
	int status = DeviceIoControl(
		h,
		FSCTL_CREATE_USN_JOURNAL,
		&cujd,
		sizeof(cujd),
		NULL,
		0,
		&br,
		NULL
	);
	if (status != 0)
		return 0;
	else
		return -1;
}

int GetUSNJournalInfo(HANDLE h, USN_JOURNAL_DATA& d)
{
	DWORD br;
	USN_JOURNAL_DATA USNInfo;
	int status = DeviceIoControl(
		h,
		FSCTL_QUERY_USN_JOURNAL,
		NULL,
		0,
		&USNInfo,
		sizeof(USNInfo),
		&br,
		NULL
	);
	if (status != 0)
	{
		d = USNInfo;
		return 0;
	}
	else
		return -1;

}

int main()
{
	const char* VolumeName = "D:";
	char FileSystemName[MAX_PATH];
	if (GetFileSystemName(FileSystemName, VolumeName) == 0)
		cout << "Get File System Name: " << FileSystemName << endl;

	HANDLE hVol;
	if (GetVolumeHandle(hVol, VolumeName) == 0)
		cout << "Get Volume Handle: " << hVol << endl;

	if (CreateUSNJournal(hVol) == 0)
	{
		cout << "Create USN Journal." << endl;
	}

	USN_JOURNAL_DATA ujd;
	if (GetUSNJournalInfo(hVol, ujd) == 0)
	{
		cout << "Get USN Journal Info." << endl;
	}

	MFT_ENUM_DATA med;
	med.StartFileReferenceNumber = 0;
	med.LowUsn = 0;
	med.HighUsn = ujd.NextUsn;

	char buffer[BUF_LEN];
	DWORD usnDataSize;
	PUSN_RECORD UsnRecord;
	SetLastError(NOERROR);
	while (DeviceIoControl(
		hVol,
		FSCTL_ENUM_USN_DATA,
		&med,
		sizeof(med),
		&buffer,
		BUF_LEN,
		&usnDataSize,
		NULL
	))
	{
		cout << 1;
		DWORD dwRetBytes = usnDataSize - sizeof(USN);

	}
	cout << 0 << endl;
	cout << GetLastError();
	return -1;


	READ_USN_JOURNAL_DATA rujd = { 0, 0xFFFFFFFF, FALSE, 0, 0 };

	DWORD dwBytes, dwRetBytes;
	rujd.UsnJournalID = ujd.UsnJournalID;
	for (int i = 0; i < 10; i++)
	{
		memset(buffer, 0, BUF_LEN);
		if (!DeviceIoControl(
			hVol,
			FSCTL_READ_USN_JOURNAL,
			&rujd,
			sizeof(rujd),
			&buffer,
			BUF_LEN,
			&dwBytes,
			NULL
		))
		{
			printf("Read journal failed (%d)\n", GetLastError());
			return -1;
		}
		dwRetBytes = dwBytes - sizeof(USN);
		UsnRecord = (PUSN_RECORD)(((PUCHAR)buffer) + sizeof(USN));
		cout << "***********************\n";
		while (dwRetBytes > 0)
		{
			printf("USN: %I64x\n", UsnRecord->Usn);
			printf("File name: %.*S\n",
				UsnRecord->FileNameLength / 2,
				UsnRecord->FileName);
			printf("Reason: %x\n", UsnRecord->Reason);
			printf("\n");

			dwRetBytes -= UsnRecord->RecordLength;

			// Find the next record
			UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) +
				UsnRecord->RecordLength);
		}
		// Update starting USN for next call
		rujd.StartUsn = *(USN*)&buffer;

	}

	return 0;
}