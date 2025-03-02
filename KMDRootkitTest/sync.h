#pragma once
#include<Ntifs.h>

namespace Sync {
	static PKDPC GainAllThreadExclusive();
	static PKDPC ReleaseAllThreadExclusive();
}