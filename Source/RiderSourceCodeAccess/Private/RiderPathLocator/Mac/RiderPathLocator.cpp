// Copyright Epic Games, Inc. All Rights Reserved.
#if PLATFORM_MAC

#include "RiderPathLocator/RiderPathLocator.h"

#include "HAL/FileManager.h"
#include "Runtime/Launch/Resources/Version.h"

static TArray<FInstallInfo> GetManuallyInstalledRiders()
{
	TArray<FInstallInfo> Result;
	TArray<FString> RiderPaths;
	IFileManager::Get().FindFilesRecursive(RiderPaths, TEXT("/Applications"), TEXT("Rider*.app"), true, false);
	for(const FString& RiderPath: RiderPaths)
	{
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(RiderPath, false);
		if(InstallInfo.IsSet())
			Result.Add(InstallInfo.GetValue());
	}
	return Result;
}

static TArray<FInstallInfo> GetRidersFromToolbox()
{
	TArray<FInstallInfo> Result;
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 20
	TCHAR CHomePath[4096];
	FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"), CHomePath, ARRAY_COUNT(CHomePath));
	const FString FHomePath = CHomePath;
#else
	const FString FHomePath = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));
#endif
	FString LocalAppData = FPaths::Combine(FHomePath, TEXT("Library"), TEXT("Application Support"));
	FString ToolboxPath = FPaths::Combine(LocalAppData, TEXT("JetBrains"), TEXT("Toolbox"));
		
	return FRiderPathLocator::GetInstallInfosFromToolbox(ToolboxPath, "Rider*.app");		
}
	
TSet<FInstallInfo> FRiderPathLocator::CollectAllPaths()
{
	TSet<FInstallInfo> InstallInfos;
	InstallInfos.Append(GetManuallyInstalledRiders());
	InstallInfos.Append(GetRidersFromToolbox());
	return InstallInfos;
}
#endif