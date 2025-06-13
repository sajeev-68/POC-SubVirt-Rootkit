#include <Windows.h>
#include "utils_client.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

std::vector<std::string> Utilities::SplitString(const std::string& str, char delimeter) {

	std::vector<std::string> result;

	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, delimeter)) {
		result.push_back(token);
	}

	return result;
}

PVARS Utilities::CreatePrivStruct(ULONG pid, std::string& Priv) {

    PVARS PrivStruct = (PVARS)malloc(sizeof(VARS));

    if (!PrivStruct) {
		return NULL;
    }

    PrivStruct->pid = pid;
    PrivStruct->Priv = Priv;

    return PrivStruct;

}

VOID Utilities::ListPrivilegesToAdd() {
      std::cout << std::left;
      std::cout<< "------------------------------------------------------------------------------------\n";
      std::cout<< "| " << std::setw(35) << "Privilege Name" << "| " << std::setw(80) << "Description" << "|\n";
      std::cout<< "------------------------------------------------------------------------------------\n";
      std::cout<< "| " << std::setw(35) << "SeCreateTokenPrivilege" << "| Allows a process to create an access token, which is used for authentication and security contexts." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeAssignPrimaryTokenPrivilege" << "| Enables a process to replace the default token associated with a running process." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeLockMemoryPrivilege" << "| Grants the ability to lock pages in physical memory, preventing them from being swapped to disk." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeIncreaseQuotaPrivilege" << "| Allows a process to increase the resource quotas of another process." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeMachineAccountPrivilege" << "| Permits a user to add computers to a domain, which is useful for domain administrators." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeTcbPrivilege" << "| Grants a process the ability to act as part of the operating system, bypassing security restrictions." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeSecurityPrivilege" << "| Enables a process to manage security auditing and access control logs." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeTakeOwnershipPrivilege" << "| Allows a user to take ownership of files, directories, or registry keys, overriding existing permissions." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeLoadDriverPrivilege" << "| Grants permission to load and unload kernel-mode drivers, which can affect system stability." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeSystemProfilePrivilege" << "| Enables profiling of system performance for diagnostic and optimization purposes." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeSystemtimePrivilege" << "| Allows modification of the system time, affecting timestamps and scheduled tasks." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeProfileSingleProcessPrivilege" << "| Grants the ability to monitor the performance of a single process." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeIncreaseBasePriorityPrivilege" << "| Permits a process to raise the base priority of other processes, affecting scheduling." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeCreatePagefilePrivilege" << "| Allows the creation and management of pagefiles, which are used for virtual memory." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeCreatePermanentPrivilege" << "| Grants permission to create objects that persist beyond the lifetime of a process." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeBackupPrivilege" << "| Allows a process to back up files and directories, ignoring access restrictions." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeRestorePrivilege" << "| Grants the ability to restore files and directories, even those owned by other users." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeShutdownPrivilege" << "| Allows a process to shut down the local system." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeDebugPrivilege" << "| Grants permission to debug and adjust the memory of other processes." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeAuditPrivilege" << "| Allows a process to generate security audit logs." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeSystemEnvironmentPrivilege" << "| Grants the ability to modify firmware environment variables." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeChangeNotifyPrivilege" << "| Allows a process to receive notifications of file or directory changes." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeRemoteShutdownPrivilege" << "| Grants permission to remotely shut down a system." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeUndockPrivilege" << "| Allows a user to undock a laptop from a docking station." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeSyncAgentPrivilege" << "| Grants access to synchronize directory service data." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeEnableDelegationPrivilege" << "| Allows a user to enable accounts for delegation in Active Directory." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeManageVolumePrivilege" << "| Grants permission to manage the files on a volume." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeImpersonatePrivilege" << "| Allows a process to impersonate a client after authentication." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeCreateGlobalPrivilege" << "| Grants the ability to create global objects in Terminal Services." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeTrustedCredManAccessPrivilege" << "| Allows access to Credential Manager as a trusted caller." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeRelabelPrivilege" << "| Grants permission to modify the mandatory integrity level of an object." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeIncreaseWorkingSetPrivilege" << "| Allows a process to allocate more physical memory for applications." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeTimeZonePrivilege" << "| Grants permission to adjust the system's time zone." << "|\n";
      std::cout<< "| " << std::setw(35) << "SeCreateSymbolicLinkPrivilege" << "| Allows a process to create symbolic links, which point to other files or directories." << "|\n";
      std::cout<< "------------------------------------------------------------------------------------\n";
}