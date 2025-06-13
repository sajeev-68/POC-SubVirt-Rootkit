#pragma once
#include <windows.h>
#include <string>
#include <vector>

typedef struct _VARS {
	ULONG pid = 0;
	std::string Priv;
}VARS, * PVARS;

namespace Utilities {
	std::vector<std::string> SplitString(const std::string& str, char delimeter);
	PVARS CreatePrivStruct(ULONG pid, std::string& Privs);
	VOID ListPrivilegesToAdd();
}