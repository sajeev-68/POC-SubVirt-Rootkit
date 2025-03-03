#include<ntddk.h>
#include<wdm.h>
#include "sync.h"
#include "utils.h"

extern LONG AllCPURaised, NumberofRaisedCPU;

static PKDPC Sync::GainAllThreadExclusive() {
	NTSTATUS ns = STATUS_SUCCESS;
	ULONG u_currentCPU = 0;
	CCHAR i;

	PKDPC dpc, temp_dpc;


	ns = Utilities::RaiseCurrentThreadLevel();

	if (!NT_SUCCESS(ns)) {
		KdPrint(("! Current thread not in Dipatch level... Unable to raise all CPU levels to DISPATCH_LEVEL\n"));
		return NULL;
	}

	InterlockedAnd(&AllCPURaised, 0);
	InterlockedAnd(&NumberofRaisedCPU, 0);

	temp_dpc = (PKDPC)ExAllocatePool(NonPagedPool, KeNumberProcessors * sizeof(KDPC));

	if (temp_dpc == NULL)
		return NULL;

	u_currentCPU = KeGetCurrentProcessorNumber();
	dpc = temp_dpc;

	for (i = 0; i < u_currentCPU; *temp_dpc++) {

		if (i != u_currentCPU) {
			KeInitializeDpc(temp_dpc, Utilities::RaiseCPUIRQLAndWait, NULL);
			KeSetTargetProcessorDpc(temp_dpc, i);
			KeInsertQueueDpc(temp_dpc, NULL, NULL);
		}
	}

	while (InterlockedCompareExchange(&NumberofRaisedCPU, KeNumberProcessors - 1, KeNumberProcessors - 1) != KeNumberProcessors - 1) {
		YieldProcessor();
	}
	return dpc;
}

static NTSTATUS Sync::ReleaseAllThreadExclusive(PVOID pkdpc) {

	InterlockedIncrement(&AllCPURaised);

	while (InterlockedCompareExchange(&NumberofRaisedCPU, 0, 0))
		YieldProcessor();

	if (pkdpc != NULL) {
		ExFreePool(pkdpc);
		pkdpc = NULL;
	}

	return STATUS_SUCCESS;
}