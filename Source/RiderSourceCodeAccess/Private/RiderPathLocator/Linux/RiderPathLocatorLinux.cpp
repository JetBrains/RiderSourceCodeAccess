// Copyright Epic Games, Inc. All Rights Reserved.

#include "RiderPathLocator/RiderPathLocator.h"

#include "Internationalization/Regex.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Launch/Resources/Version.h"

#if PLATFORM_LINUX

TOptional<FInstallInfo> FRiderPathLocator::GetInstallInfoFromRiderPath(const FString& Path, FInstallInfo::EInstallType InstallType)
{
	if(!FPaths::FileExists(Path))
	{
		return {};
	}
	
	const FString PatternString(TEXT("(.*)(?:\\\\|/)bin"));
	const FRegexPattern Pattern(PatternString);
	FRegexMatcher RiderPathMatcher(Pattern, Path);
	if (!RiderPathMatcher.FindNext())
	{
		return {};
	}

	const FString RiderDir = RiderPathMatcher.GetCaptureGroup(1);
	const FString RiderCppPluginPath = FPaths::Combine(RiderDir, TEXT("plugins"), TEXT("rider-cpp"));
	if (!FPaths::DirectoryExists(RiderCppPluginPath))
	{
		return {};
	}
	
	FInstallInfo Info;
	Info.Path = Path;
	Info.InstallType = InstallType;
	const FString ProductInfoJsonPath = FPaths::Combine(RiderDir, TEXT("product-info.json"));
	if (FPaths::FileExists(ProductInfoJsonPath))
	{
		ParseProductInfoJson(Info, ProductInfoJsonPath);
	}
	if(!Info.Version.IsInitialized())
	{
		Info.Version = FPaths::GetBaseFilename(RiderDir);
		if(Info.Version.Major() >= 221)
		{
			Info.SupportUprojectState = FInstallInfo::ESupportUproject::Release;
		}
	}
	return Info;
}

static FString GetHomePath()
{
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 20
	TCHAR CHomePath[4096];
	FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"), CHomePath, ARRAY_COUNT(CHomePath));
	const FString FHomePath = CHomePath;
#else
	const FString FHomePath = FPlatformMisc::GetEnvironmentVariable(TEXT("HOME"));
#endif

	return FHomePath;
}

static TArray<FInstallInfo> GetManuallyInstalledRiders()
{
	TArray<FInstallInfo> Result;
	TArray<FString> RiderPaths;

	const FString FHomePath = GetHomePath();
	const FString HomePathMask = FPaths::Combine(FHomePath, TEXT("Rider.sh"));

	IFileManager::Get().FindFiles(RiderPaths, *HomePathMask, false, true);

	for(const FString& RiderPath: RiderPaths)
	{
		FString FullPath = FPaths::Combine(FHomePath, RiderPath);
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(FullPath, FInstallInfo::EInstallType::Installed);
		if(InstallInfo.IsSet())
		{
			Result.Add(InstallInfo.GetValue());
		}
	}

	const FString FOptPath = TEXT("/opt");
	const FString OptPathMask = FPaths::Combine(FOptPath, TEXT("Rider.sh"));

	IFileManager::Get().FindFiles(RiderPaths, *OptPathMask, false, true);

	for(const FString& RiderPath: RiderPaths)
	{
		FString FullPath = FPaths::Combine(FOptPath, RiderPath);
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
	FString LocalAppData = FPaths::Combine(GetHomePath(), TEXT(".local"), TEXT("share"));

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
	InstallInfos.Append(GetInstallInfosFromToolbox(GetToolboxPath(), "Rider.sh"));
	InstallInfos.Append(GetInstallInfosFromResourceFile());
	return InstallInfos;
}
#endif
