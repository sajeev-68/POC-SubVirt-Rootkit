#include "utils.h"
#include "offsets.h"
#include <ntstatus.h>
#include <ntddk.h>
#include <ntstrsafe.h>

extern LONG AllCPURaised, NumberofRaisedCPU;

PEPROCESS Utilities::FindProcEproc(ULONG target_pid) {

	PEPROCESS eproc;

	ULONG current_pid = 0;
	ULONG start_pid = 0;
	int i_count = 0;

	PLIST_ENTRY plist_active_procs;

	//
	// return if tagret process is 0
	//
	if (target_pid == 0) {
		return NULL;
	}

	//
	// Get current process pid
	//
	eproc = PsGetCurrentProcess();
	start_pid = *((int*)((PUCHAR)eproc + PIDOFFSET));
	current_pid = start_pid;

	while (1) {
		//
		// return found eproc if PID is found
		//
		if (target_pid == current_pid)
			return eproc;

		//
		// return 0 if we reach the starting PID and iteration is greater than one (Circular linked list strucutre)
		//
		else if ((i_count >= 1) && (start_pid == current_pid))
			return 0;

		else {
			//
			// iterate thorough the linked list
			//
			plist_active_procs = (PLIST_ENTRY)((PUCHAR)eproc + FLINK_OFFSET);
			plist_active_procs = plist_active_procs->Flink;
			eproc = (PEPROCESS)((PUCHAR)plist_active_procs-FLINK_OFFSET);
			current_pid = *((int*)((PUCHAR)eproc + PIDOFFSET));
			i_count++;
		}
	}

}

NTSTATUS Utilities::RaiseCurrentThreadLevel(KIRQL* OldIRQL) {
	KIRQL CurrentIRQL;

	CurrentIRQL = KeGetCurrentIrql();
	*OldIRQL = CurrentIRQL;

	if (CurrentIRQL < DISPATCH_LEVEL)
		KeRaiseIrql(DISPATCH_LEVEL, OldIRQL);

	if (KeGetCurrentIrql() != DISPATCH_LEVEL) {
		KdPrint(("! Failed to raise IRQL level to DISPATCH_LEVEL..."));
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

VOID Utilities::RaiseCPUIRQLAndWait(IN PKDPC Dpc, IN PVOID DefferedContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2) {
	UNREFERENCED_PARAMETER(SystemArgument2);
	UNREFERENCED_PARAMETER(SystemArgument1);
	UNREFERENCED_PARAMETER(DefferedContext);
	UNREFERENCED_PARAMETER(Dpc);

	InterlockedIncrement(&NumberofRaisedCPU);

	while (!InterlockedCompareExchange(&AllCPURaised, 1, 1))
	{
		YieldProcessor();
	}
	InterlockedDecrement(&NumberofRaisedCPU);

}

NTSTATUS Utilities::DropCurrentThreadLevel(KIRQL OldIRQL) {

	KIRQL CurrentIRQL;
	CurrentIRQL = KeGetCurrentIrql();
	if (CurrentIRQL == DISPATCH_LEVEL)
		KeLowerIrql(OldIRQL);

	if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
		KdPrint(("! Failed to lower IRQL level to DISPATCH_LEVEL..."));
		return STATUS_UNSUCCESSFUL;
	}
	return STATUS_SUCCESS;
}
