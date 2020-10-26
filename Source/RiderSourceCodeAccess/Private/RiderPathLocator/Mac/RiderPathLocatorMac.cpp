// Copyright Epic Games, Inc. All Rights Reserved.
#if PLATFORM_MAC
#include "RiderPathLocator/RiderPathLocator.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Launch/Resources/Version.h"

TOptional<FInstallInfo> FRiderPathLocator::GetInstallInfoFromRiderPath(const FString& PathToRiderApp, bool bIsToolbox)
{
	if(!FPaths::DirectoryExists(PathToRiderApp)) return {};

	const FString RiderCppPluginPath = FPaths::Combine(PathToRiderApp, TEXT("Contents"), TEXT("plugins"), TEXT("rider-cpp"));
	if (!FPaths::DirectoryExists(RiderCppPluginPath)) return {};
	
	FInstallInfo Info;
	Info.Path = FPaths::Combine(PathToRiderApp, TEXT("Contents"), TEXT("MacOS"), TEXT("rider"));
	Info.IsToolbox = bIsToolbox;
	const FString ProductInfoJsonPath = FPaths::Combine(PathToRiderApp, TEXT("Contents"), TEXT("Resources"), TEXT("product-info.json"));
	if (FPaths::FileExists(ProductInfoJsonPath))
	{
		ParseProductInfoJson(Info, ProductInfoJsonPath);
	}
	return Info;
}

static TArray<FInstallInfo> GetManuallyInstalledRiders()
{
	TArray<FInstallInfo> Result;
	TArray<FString> RiderPaths;
	IFileManager::Get().FindFilesRecursive(RiderPaths, TEXT("/Applications"), TEXT("Rider*.app"), false, true);
	for(const FString& RiderPath: RiderPaths)
	{
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(RiderPath, false);
		if(InstallInfo.IsSet())
			Result.Add(InstallInfo.GetValue());
	}
	return Result;
}

static FString GetToolboxPath()
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

	return FPaths::Combine(LocalAppData, TEXT("JetBrains"), TEXT("Toolbox"));
}
	
TSet<FInstallInfo> FRiderPathLocator::CollectAllPaths()
{
	TSet<FInstallInfo> InstallInfos;
	InstallInfos.Append(GetManuallyInstalledRiders());
	InstallInfos.Append(GetInstallInfosFromToolbox(GetToolboxPath(), "Rider*.app"));
	return InstallInfos;
}
#endif