// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace FRiderPathLocator
{
	struct FInstallInfo
	{
		FString Path;
		FString Version;
		bool IsToolbox = false;

		bool operator<(const FInstallInfo& rhs) const
		{
			return Version < rhs.Version;
		}

		bool operator==(const FInstallInfo& rhs) const
		{
			return !(*this < rhs) && !(rhs < *this); 
		}
	};

	TSet<FInstallInfo> CollectAllPaths();
}
		
FORCEINLINE uint32 GetTypeHash(const FRiderPathLocator::FInstallInfo& InstallInfo)
{
	return GetTypeHash(InstallInfo.Path);
}