// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

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

class FRiderPathLocator
{
public:
	static TArray<FInstallInfo> GetInstallInfosFromToolbox(const FString& ToolboxPath, const FString& Pattern);
	static TOptional<FInstallInfo> GetInstallInfoFromRiderPath(FString Path, bool bIsToolbox);
	
	// Platform specific implementation
	static TSet<FInstallInfo> CollectAllPaths();
private:	

	static TArray<FInstallInfo> GetInstallInfos(const FString& ToolboxRiderRootPath, const FString& Pattern, bool IsToolbox);
};
		
FORCEINLINE uint32 GetTypeHash(const FInstallInfo& InstallInfo)
{
	return GetTypeHash(InstallInfo.Path);
}