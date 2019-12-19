#include<Windows.h>
#include<iostream>

using namespace std;

int GetFileSystemName(char *FileSystemName, const char *RootPathName = "D:")
{
	char FileSystemNameBuffer[MAX_PATH];
	int status = GetVolumeInformationA(
		RootPathName,
		NULL,
		NULL,
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

int GetVolumeHandle(HANDLE &handle, const char* RootPathName = "D:")
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

int main()
{
	const char* VolumeName = "D:";
	char FileSystemName[MAX_PATH];
	if (GetFileSystemName(FileSystemName, VolumeName) == 0)
		cout << FileSystemName << endl;

	HANDLE h;
	if (GetVolumeHandle(h, VolumeName) == 0)
		cout << h << endl;
	


	return 0;
}