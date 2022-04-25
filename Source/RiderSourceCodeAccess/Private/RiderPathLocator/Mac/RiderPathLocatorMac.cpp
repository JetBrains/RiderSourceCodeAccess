// Copyright Epic Games, Inc. All Rights Reserved.

#include "RiderPathLocator/RiderPathLocator.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Launch/Resources/Version.h"

#if PLATFORM_MAC

TOptional<FInstallInfo> FRiderPathLocator::GetInstallInfoFromRiderPath(const FString& PathToRiderApp, FInstallInfo::EInstallType InstallType)
{
	if(!FPaths::DirectoryExists(PathToRiderApp))
	{
		return {};
	}

	const FString RiderCppPluginPath = FPaths::Combine(PathToRiderApp, TEXT("Contents"), TEXT("plugins"), TEXT("rider-cpp"));
	if (!FPaths::DirectoryExists(RiderCppPluginPath))
	{
		return {};
	}

	FInstallInfo Info;
	Info.Path = FPaths::Combine(PathToRiderApp, TEXT("Contents"), TEXT("MacOS"), TEXT("rider"));
	Info.InstallType = InstallType;
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
	IFileManager::Get().FindFiles(RiderPaths, TEXT("/Applications/Rider*.app"), false, true);
	for(const FString& RiderPath: RiderPaths)
	{
		FString FullPath = TEXT("/Applications/") + RiderPath;
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(FullPath, FInstallInfo::EInstallType::Installed);
		if(InstallInfo.IsSet())
		{
			Result.Add(InstallInfo.GetValue());
		}
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

static TArray<FInstallInfo> GetInstalledRidersWithMdfind()
{
	int32 ReturnCode;
	FString OutResults;
	FString OutErrors;

	// avoid trying to run mdfind if it doesnt exists
	if (!FPaths::FileExists(TEXT("/usr/bin/mdfind")))
	{
		return {};
	}

	FPlatformProcess::ExecProcess(TEXT("/usr/bin/mdfind"), TEXT("\"kMDItemKind == Application\""), &ReturnCode, &OutResults, &OutErrors);
	if (ReturnCode != 0)
	{
		return {};
	}

	TArray<FString> RiderPaths;
	FString TmpString;
	while(OutResults.Split(TEXT("\n"), &TmpString, &OutResults))
	{
		if(TmpString.Contains(TEXT("Rider")))
		{
			RiderPaths.Add(TmpString);
		}
	}
	TArray<FInstallInfo> Result;
	for(const FString& RiderPath: RiderPaths)
	{
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(RiderPath, FInstallInfo::EInstallType::Installed);
		if(InstallInfo.IsSet())
		{
			Result.Add(InstallInfo.GetValue());
		}
	}
	return Result;
}

TSet<FInstallInfo> FRiderPathLocator::CollectAllPaths()
{
	TSet<FInstallInfo> InstallInfos;
	InstallInfos.Append(GetInstalledRidersWithMdfind());
	InstallInfos.Append(GetManuallyInstalledRiders());
	InstallInfos.Append(GetInstallInfosFromToolbox(GetToolboxPath(), "Rider*.app"));
	InstallInfos.Append(GetInstallInfosFromResourceFile());
	return InstallInfos;
}
#endif
