#pragma once
#include <string>
#include <windows.h>

bool ChangeVSN(const std::string& drive, DWORD newSerial);
DWORD GetVSN(const std::string& drive);