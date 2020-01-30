#pragma once

#include "CoreMinimal.h"

namespace FRiderPathLocator
{
	struct FInstallInfo
	{
		FString Path;
		FString Version;
		bool IsToolbox = false;
	};
	TArray<FInstallInfo> CollectAllPaths();
}
