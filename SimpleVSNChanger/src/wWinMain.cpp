#include "VSNOperations.h"
#include <iostream>
#include <fstream>
#include <conio.h>
#include <iomanip>

#pragma warning(push)
#pragma warning(disable : 6031) // freopen return value skip warning

static void ClearConsole(HANDLE hConsole) {
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi{};
		DWORD count{};
		DWORD cellCount{};
		COORD zeroCoord = { 0, 0 };

		if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
			return;
		cellCount = csbi.dwSize.X * csbi.dwSize.Y;

		FillConsoleOutputCharacterW(hConsole, L' ', cellCount, zeroCoord, &count);
		FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, zeroCoord, &count);
		SetConsoleCursorPosition(hConsole, zeroCoord);
	}
}

using std::cout;

int main(int argc, char* argv[]) {
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (argv[i] == "-n" && argc >= i + 1) {
				std::string VSNcandidate = argv[i + 1];
				if (VSNcandidate.empty()) {
					cout << "Wrong serial number format.";
					return 1;
				}
				// To lowercase
				for (char& c : VSNcandidate)
					c += 32i8 * (c >= 'A' && c <= 'Z');

				if (VSNcandidate.find('-') != std::string::npos)
					VSNcandidate.erase(std::remove(VSNcandidate.begin(), VSNcandidate.end(), '-'), VSNcandidate.end());

				for (char c : VSNcandidate) {
					if (c < '0' || c > 'f' || (c > '9' && c < 'a')) {
						cout << "Wrong serial number format.";
						return 2;
					}
				}
				if (!ChangeVSN("C", stoul(VSNcandidate, nullptr, 16))) {
					cout << "Error changing the volume serial number!";
					return 3;
				}
				cout << "The volume serial number is successfully changed, please restart your windows machine for the changes to take effect!";
				return 0;
			}
		}
	}

start:
	ClearConsole(GetStdHandle(STD_OUTPUT_HANDLE));


	cout << "Welcome to a simple Volume Serial Number changer\n\n";
	cout << "1 = Print the serial number;\n2 = Backup serial number to file\n3 = Change serial number\n4 = Change serial number from file\n5 / q = Exit\nYour choice: ";


	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode{};
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));

	WCHAR response;
	INPUT_RECORD record{};
	DWORD read{};
	while (true) {
		ReadConsoleInputW(hStdin, &record, 1, &read);
		if (record.EventType == KEY_EVENT && record.Event.KeyEvent.bKeyDown) {
			WCHAR wch = record.Event.KeyEvent.uChar.UnicodeChar;
			if (wch >= L'1' && wch <= L'5' || wch == 'q') {
				response = wch;
				break;
			}
		}
		Sleep(1);
	}
	SetConsoleMode(hStdin, mode);
	ClearConsole(GetStdHandle(STD_OUTPUT_HANDLE));

	if (response == L'5' || response == 'q') {
		return 0;
	}
	if (response == L'1') {
		DWORD VSN = GetVSN("C");
		if (!VSN) {
			cout << "Error while getting Volume Serial Number!\nPress any key to restart the program..";
			_getch();
			goto start;
		}
		cout << "Volume serial number of the C drive is " << std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0') << ((uint16_t)(VSN >> 16)) << "-"
			<< std::setw(4) << std::setfill('0') << ((uint16_t)(VSN & 0xFFFFu))
			<< " (0x" << std::setw(8) << std::setfill('0') << VSN << ")\n\nPress any key to restart the program..";
		_getch();
		goto start;
	}
	if (response == L'2') {
		OPENFILENAMEW ofn{ sizeof(OPENFILENAMEW) };
		WCHAR fileName[MAX_PATH] = L"VolumeSerialNumber.txt";

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0";
		ofn.nFilterIndex = 2;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;

		if (!GetSaveFileNameW(&ofn)) {
			cout << "User canceled the save.\nPress any key to restart the program..";
			_getch();
			goto start;
		}
		DWORD VSN = GetVSN("C");
		std::ofstream(ofn.lpstrFile) << std::hex << std::uppercase
			<< std::setw(4) << std::setfill('0') << ((VSN >> 16) & 0xFFFF)
			<< "-"
			<< std::setw(4) << std::setfill('0') << (VSN & 0xFFFF);
		cout << "File saved.\nPress any key to restart the program..";
		_getch();
		goto start;
	}
	if (response == L'3') {
		bool continueFlag = false;
		std::string VSNcandidate;
		cout << "Please enter the serial number: ";
		std::getline(std::cin, VSNcandidate);

		while (true) {
			if (VSNcandidate.empty()) {
				continueFlag = true;
				cout << "Wrong serial number, please enter a valid serial number: ";
				std::getline(std::cin, VSNcandidate);
				break;
			}

			// To lowercase
			for (char& c : VSNcandidate)
				c += 32i8 * (c >= 'A' && c <= 'Z');

			if (VSNcandidate.find('-') != std::string::npos)
				VSNcandidate.erase(std::remove(VSNcandidate.begin(), VSNcandidate.end(), '-'), VSNcandidate.end());

			for (char c : VSNcandidate) {
				if (c < '0' || c > 'f' || (c > '9' && c < 'a')) {
					continueFlag = true;
					cout << "Wrong serial number, please enter a valid serial number: ";
					std::getline(std::cin, VSNcandidate);
					break;
				}
			}
			if (!continueFlag) {
				break;
			}
			continueFlag = false;
		}
		if (!ChangeVSN("C", stoul(VSNcandidate, nullptr, 16))) {
			cout << "Error changing the volume serial number!\nPress any key to restart the program..";
			_getch();
			goto start;
		}
		cout << "The volume serial number is successfully changed, please restart your windows machine for the changes to take effect!\n\nPress any key to restart the program..";
		_getch();
		goto start;
	}
	if (response == L'4') {
		OPENFILENAMEW ofn{ sizeof(OPENFILENAMEW) };
		WCHAR fileName[MAX_PATH] = L"VolumeSerialNumber.txt";

		ofn.lpstrFile = fileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"All Files\0*.*\0Text Files\0*.txt\0";
		ofn.nFilterIndex = 2;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

		if (!GetOpenFileNameW(&ofn)) {
			cout << "User canceled opening the file.\nPress any key to restart the program..";
			_getch();
			goto start;
		}

		HANDLE hFile = CreateFileW(
			ofn.lpstrFile, GENERIC_READ,
			FILE_SHARE_READ, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr
		);

		if (hFile == INVALID_HANDLE_VALUE) {
			cout << "Error opening file to read.\nPress any key to restart the program..";
			_getch();
			goto start;
		}

		std::string VSNcandidate;
		{
			char buffer[16]{};
			DWORD bytesWritten;
			ReadFile(hFile, buffer, 16, &bytesWritten, nullptr);
			VSNcandidate = buffer;
		}
		CloseHandle(hFile);

		if (VSNcandidate.empty()) {
			cout << "Wrong serial number format.\nPress any key to restart the program..";
			_getch();
			goto start;
		}
		// To lowercase
		for (char& c : VSNcandidate)
			c += 32i8 * (c >= 'A' && c <= 'Z');

		if (VSNcandidate.find('-') != std::string::npos)
			VSNcandidate.erase(std::remove(VSNcandidate.begin(), VSNcandidate.end(), '-'), VSNcandidate.end());

		for (char c : VSNcandidate) {
			if (c < '0' || c > 'f' || (c > '9' && c < 'a')) {
				cout << "Wrong serial number format.\nPress any key to restart the program..";
				_getch();
				goto start;
			}
		}
		if (!ChangeVSN("C", stoul(VSNcandidate, nullptr, 16))) {
			cout << "Error changing the volume serial number!\nPress any key to restart the program..";
			_getch();
			goto start;
		}
		cout << "The volume serial number is successfully changed, please restart your windows machine for the changes to take effect!\n\nPress any key to restart the program..";
		_getch();
		goto start;
	}
}
#pragma warning(pop)