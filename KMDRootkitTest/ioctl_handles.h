#pragma once
#include <Ntifs.h>

namespace IoctlHandlers {

	NTSTATUS HandleHideProcess(PIRP pIrp);
	NTSTATUS HandleElevate(PIRP pIrp);
	NTSTATUS HandleSubvirt();
	NTSTATUS HandleTestConnection(PIRP pIrp, ULONG bufferSize);
}
