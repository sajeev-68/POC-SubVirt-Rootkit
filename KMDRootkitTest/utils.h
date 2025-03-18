#pragma once
#include <ntddk.h>

static LONG AllCPURaised, NumberofRaisedCPU;

namespace Utilities {
	PEPROCESS FindProcEproc(ULONG target_pid);
	NTSTATUS RaiseCurrentThreadLevel(KIRQL* oldIRQL);
	NTSTATUS DropCurrentThreadLevel(KIRQL oldIRQL);
	VOID RaiseCPUIRQLAndWait(IN PKDPC Dpc, IN PVOID DefferedContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
}