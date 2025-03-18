#include<ntddk.h>
#include "sync.h"
#include "utils.h"

extern LONG AllCPURaised, NumberofRaisedCPU;
KIRQL oldIRQL;

PKDPC Sync::GainAllThreadExclusive() {
	NTSTATUS ns = STATUS_SUCCESS;
	LONG u_currentCPU = 0;
	CCHAR i;
	
	PKDPC dpc, temp_dpc;


	ns = Utilities::RaiseCurrentThreadLevel(&oldIRQL);

	if (!NT_SUCCESS(ns)) {
		DbgPrintEx(0, 0, "! Current thread not in Dipatch level... Unable to raise all CPU levels to DISPATCH_LEVEL\n");
		return NULL;
	}

	InterlockedAnd(&AllCPURaised, 0);
	InterlockedAnd(&NumberofRaisedCPU, 0);

	temp_dpc = (PKDPC)ExAllocatePool2(NonPagedPool, KeNumberProcessors * sizeof(KDPC), 2);

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

NTSTATUS Sync::ReleaseAllThreadExclusive(PVOID pkdpc) {

	NTSTATUS ns = STATUS_SUCCESS;

	DbgPrintEx(0, 0, "ReleaseAllThreadExclusive called at IRQL: %d\n", KeGetCurrentIrql());

	InterlockedIncrement(&AllCPURaised);

	while (InterlockedCompareExchange(&NumberofRaisedCPU, 0, 0))
		YieldProcessor();

	if (pkdpc != NULL) {
		ExFreePool(pkdpc);
		pkdpc = NULL;
	}
	ns = Utilities::DropCurrentThreadLevel(oldIRQL);

	if (!NT_SUCCESS(ns)) {
		DbgPrintEx(0, 0, "! Current thread not in Passive level...\n");
		return NULL;
	}
	DbgPrintEx(0, 0, "Current IRQL: %d\n", KeGetCurrentIrql());
	return STATUS_SUCCESS;
}