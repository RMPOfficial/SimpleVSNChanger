#include "VSNOperations.h"

bool ChangeVSN(const std::string& drive, DWORD newSerial)
{
	HANDLE hDrive = CreateFileA(
		("\\\\.\\" + drive + ":").c_str(),
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if (hDrive == INVALID_HANDLE_VALUE) {
		return false;
	}

	BYTE sector[512]{};
	DWORD bytesRead{};

	if (!ReadFile(hDrive, sector, 512, &bytesRead, NULL)) {
		CloseHandle(hDrive);
		return false;
	}

	*(DWORD*)&sector[72] = newSerial;

	DWORD bytesWritten{};
	SetFilePointer(hDrive, 0, NULL, FILE_BEGIN);
	if (!WriteFile(hDrive, sector, 512, &bytesWritten, NULL)) {
		CloseHandle(hDrive);
		return false;
	}

	CloseHandle(hDrive);
	return true;
}

DWORD GetVSN(const std::string& drive)
{
	DWORD serialNumber{};
	if (GetVolumeInformationA(
		(drive + ":\\").c_str(),
		NULL, 0,
		&serialNumber,
		NULL, NULL, NULL, 0
	))
		return serialNumber;

	return 0;
}
