#pragma once
#include <ntddk.h>
#include <ntdef.h>

static LONG AllCPURaised, NumberofRaisedCPU;

typedef struct _PRIVILEGE_MAPPING {
    PCSTR PrivilegeName;
    ULONG PrivilegeConstant;
} PRIVILEGE_MAPPING, * PPRIVILEGE_MAPPING;

typedef struct _EX_FAST_REF{
	union
	{
		PVOID Object;
		ULONG RefCnt : 3;
		ULONG Value;
	};
} EX_FAST_REF, * PEX_FAST_REF;

typedef struct _SEP_TOKEN_PRIVILEGES{
	ULONG Present;
	ULONG Enabled;
	ULONG EnabledByDefault;

} SEP_TOKEN_PRIVILEGES, * PSEP_TOKEN_PRIVILEGES; 

typedef struct _TEMP_VARS {

	ULONG pid;
	STRING Priv;

}TEMP_VARS, *PTEMP_VARS ;

typedef struct _VARS {

	ULONG pid;
	ULONG Present_Mask;
	ULONG Enabled_Mask;
	ULONG EnabledByDefault_Mask;
	
}VARS, * PVARS;

namespace Utilities {
	PEPROCESS FindProcEproc(ULONG target_pid);
	NTSTATUS RaiseCurrentThreadLevel(KIRQL* oldIRQL);
	NTSTATUS DropCurrentThreadLevel(KIRQL oldIRQL);
	VOID RaiseCPUIRQLAndWait(IN PKDPC Dpc, IN PVOID DefferedContext, IN PVOID SystemArgument1, IN PVOID SystemArgument2);
	ULONG_PTR GetProcessToken(PEPROCESS eproc);
	PSEP_TOKEN_PRIVILEGES GetTokenSEP(ULONG_PTR token);
	PVARS GetVars(PTEMP_VARS temp_privs, ULONG Old_Present, ULONG Old_enabled, ULONG Old_enabledbydef);
}