#pragma once
#include<Ntifs.h>

namespace Sync {
	static PKDPC GainAllThreadExclusive();
	static NTSTATUS ReleaseAllThreadExclusive(PVOID pkdpc);
}