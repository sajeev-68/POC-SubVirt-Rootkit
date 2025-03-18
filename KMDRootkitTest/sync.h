#pragma once

namespace Sync {
	PKDPC GainAllThreadExclusive();
	NTSTATUS ReleaseAllThreadExclusive(PVOID pkdpc);
}