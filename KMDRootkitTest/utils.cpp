#include<Ntstrsafe.h>
#include "utils.h"
#include "offsets_n_tokens.h"

static LONG AllCPURaised, NumberofRaisedCPU;

static DWORD Utilities::FindProcEproc(ULONG target_pid) {
	DWORD eproc = 0x0000000000000000;

	ULONG current_pid = 0;
	ULONG start_pid = 0;
	int i_count = 0;

	PLIST_ENTRY plist_active_procs;

	//
	// return if tagret process is 0
	//
	if (target_pid == 0) {
		return target_pid;
	}

	//
	// Get current process pid
	//
	eproc = (DWORD)PsGetCurrentProcess();
	start_pid = *((ULONG*)(eproc + PIDOFFSET));
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
			return 0x0000000000000000;

		else {
			//
			// iterate thorough the linked list
			//
			plist_active_procs = (LIST_ENTRY*)(eproc + FLINK_OFFSET);
			eproc = (DWORD)plist_active_procs->Flink;
			eproc = eproc - FLINK_OFFSET;
			current_pid = *((int*)(eproc + PIDOFFSET));
			i_count++;
		}
	}

}

static NTSTATUS Utilities::RaiseCurrentThreadLevel() {
	KIRQL CurrentIRQL, OldIRQL;

	CurrentIRQL = KeGetCurrentIrql();
	OldIRQL = CurrentIRQL;

	if (CurrentIRQL < DISPATCH_LEVEL)
		KeRaiseIrql(DISPATCH_LEVEL, &OldIRQL);

	if (KeGetCurrentIrql() != DISPATCH_LEVEL) {
		KdPrint(("! Failed to raise IRQL level to DISPATCH_LEVEL..."));
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;
}

static VOID Utilities::RaiseCPUIRQLAndWait(IN PKDPC Dpc, IN PVOID DefferedContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2) {
	InterlockedIncrement(&NumberofRaisedCPU);

	while (!InterlockedCompareExchange(&AllCPURaised, 1, 1))
	{
		_asm nop;
	}
	InterlockedDecrement(&NumberofRaisedCPU);

}
