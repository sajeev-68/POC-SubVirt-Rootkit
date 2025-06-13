#include "utils.h"
#include "offsets.h"
#include <ntstatus.h>
#include <ntddk.h>
#include <ntstrsafe.h>

extern LONG AllCPURaised, NumberofRaisedCPU;


PRIVILEGE_MAPPING PrivilegeTable[] = {
   {"SeCreateTokenPrivilege",              SE_CREATE_TOKEN_PRIVILEGE},
   {"SeAssignPrimaryTokenPrivilege",       SE_ASSIGNPRIMARYTOKEN_PRIVILEGE},
   {"SeLockMemoryPrivilege",               SE_LOCK_MEMORY_PRIVILEGE},
   {"SeIncreaseQuotaPrivilege",            SE_INCREASE_QUOTA_PRIVILEGE},
   {"SeMachineAccountPrivilege",           SE_MACHINE_ACCOUNT_PRIVILEGE},
   {"SeTcbPrivilege",                      SE_TCB_PRIVILEGE},
   {"SeSecurityPrivilege",                 SE_SECURITY_PRIVILEGE},
   {"SeTakeOwnershipPrivilege",            SE_TAKE_OWNERSHIP_PRIVILEGE},
   {"SeLoadDriverPrivilege",               SE_LOAD_DRIVER_PRIVILEGE},
   {"SeSystemProfilePrivilege",            SE_SYSTEM_PROFILE_PRIVILEGE},
   {"SeSystemtimePrivilege",               SE_SYSTEMTIME_PRIVILEGE},
   {"SeProfileSingleProcessPrivilege",     SE_PROF_SINGLE_PROCESS_PRIVILEGE},
   {"SeIncreaseBasePriorityPrivilege",     SE_INC_BASE_PRIORITY_PRIVILEGE},
   {"SeCreatePagefilePrivilege",           SE_CREATE_PAGEFILE_PRIVILEGE},
   {"SeCreatePermanentPrivilege",          SE_CREATE_PERMANENT_PRIVILEGE},
   {"SeBackupPrivilege",                   SE_BACKUP_PRIVILEGE},
   {"SeRestorePrivilege",                  SE_RESTORE_PRIVILEGE},
   {"SeShutdownPrivilege",                 SE_SHUTDOWN_PRIVILEGE},
   {"SeDebugPrivilege",                    SE_DEBUG_PRIVILEGE},
   {"SeAuditPrivilege",                    SE_AUDIT_PRIVILEGE},
   {"SeSystemEnvironmentPrivilege",        SE_SYSTEM_ENVIRONMENT_PRIVILEGE},
   {"SeChangeNotifyPrivilege",             SE_CHANGE_NOTIFY_PRIVILEGE},
   {"SeRemoteShutdownPrivilege",           SE_REMOTE_SHUTDOWN_PRIVILEGE},
   {"SeUndockPrivilege",                   SE_UNDOCK_PRIVILEGE},
   {"SeSyncAgentPrivilege",                SE_SYNC_AGENT_PRIVILEGE},
   {"SeEnableDelegationPrivilege",         SE_ENABLE_DELEGATION_PRIVILEGE},
   {"SeManageVolumePrivilege",             SE_MANAGE_VOLUME_PRIVILEGE},
   {"SeImpersonatePrivilege",              SE_IMPERSONATE_PRIVILEGE},
   {"SeCreateGlobalPrivilege",             SE_CREATE_GLOBAL_PRIVILEGE},
   {"SeTrustedCredManAccessPrivilege",     SE_TRUSTED_CREDMAN_ACCESS_PRIVILEGE},
   {"SeRelabelPrivilege",                  SE_RELABEL_PRIVILEGE},
   {"SeIncreaseWorkingSetPrivilege",       SE_INC_WORKING_SET_PRIVILEGE},
   {"SeTimeZonePrivilege",                 SE_TIME_ZONE_PRIVILEGE},
   {"SeCreateSymbolicLinkPrivilege",       SE_CREATE_SYMBOLIC_LINK_PRIVILEGE}
};

static BOOLEAN GetPrivilegeConstant(PCSTR PrivilegeName, PULONG PrivilegeConstant) {
	SIZE_T i;
	for (i = 0; i < ARRAYSIZE(PrivilegeTable); i++) {
		if (strcmp(PrivilegeName, PrivilegeTable[i].PrivilegeName) == 0) {
			*PrivilegeConstant = PrivilegeTable[i].PrivilegeConstant;
			return TRUE;
		}
	}
	return FALSE;
}

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

ULONG_PTR Utilities::GetProcessToken(PEPROCESS eproc) {

	ULONG_PTR token;
	token = *(ULONG_PTR*)((PUCHAR)eproc + TOKENOFFSET);
	
	token = token & 0xFFFFFFFFFFFFFFF0;

	return token;

}

PSEP_TOKEN_PRIVILEGES Utilities::GetTokenSEP(ULONG_PTR token) {
	
	PSEP_TOKEN_PRIVILEGES sep;
	sep = (SEP_TOKEN_PRIVILEGES*)((PUCHAR)token + SEPOFFSET);
	return sep;
} 

PVARS Utilities::GetVars(PTEMP_VARS temp_privs, ULONG Old_Present, ULONG Old_enabled, ULONG Old_enabledbydef) {
	PVARS vars = (PVARS)ExAllocatePool2(NonPagedPool, sizeof(VARS), '3');

	if (!vars) {
		return NULL;
	}

	STRING priv = temp_privs->Priv;
	ULONG pid = temp_privs->pid;

	ULONG Present_Mask = Old_Present | GetPrivilegeConstant(priv.Buffer, &Present_Mask);
	ULONG Enabled_Mask = Old_enabled | GetPrivilegeConstant(priv.Buffer, &Enabled_Mask);
	ULONG EnabledByDefault_Mask = Old_enabledbydef | GetPrivilegeConstant(priv.Buffer, &EnabledByDefault_Mask);

	vars->pid = pid;
	vars->Present_Mask = Present_Mask;
	vars->Enabled_Mask = Enabled_Mask;
	vars->EnabledByDefault_Mask = EnabledByDefault_Mask;

	
	return vars;
}
