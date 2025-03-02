#pragma once
#include <Ntifs.h>
#include<ntstrsafe.h>

namespace Utilities {
	static DWORD FindProcEproc(ULONG target_pid);
	static NTSTATUS RaiseCurrentThreadLevel();
	static VOID RaiseCPUIRQLAndWait(IN PKDPC Dpc, IN PVOID DefferedContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
}