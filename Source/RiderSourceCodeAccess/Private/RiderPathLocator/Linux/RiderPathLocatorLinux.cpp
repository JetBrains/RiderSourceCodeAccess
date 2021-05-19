#include "RiderPathLocator/RiderPathLocator.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Launch/Resources/Version.h"

#if PLATFORM_LINUX

TOptional<FInstallInfo> FRiderPathLocator::GetInstallInfoFromRiderPath(const FString& PathToRiderApp, bool bIsToolbox)
{
	if(!FPaths::DirectoryExists(PathToRiderApp)) return {};

	const FString RiderCppPluginPath = FPaths::Combine(PathToRiderApp, TEXT("plugins"), TEXT("rider-cpp"));

	if (!FPaths::DirectoryExists(RiderCppPluginPath)) return {};
	
	FInstallInfo Info;
	Info.Path = FPaths::Combine(PathToRiderApp, TEXT("bin"), TEXT("rider.sh"));
	Info.IsToolbox = bIsToolbox;
	const FString ProductInfoJsonPath = FPaths::Combine(PathToRiderApp, TEXT("product-info.json"));
	if (FPaths::FileExists(ProductInfoJsonPath))
	{
		ParseProductInfoJson(Info, ProductInfoJsonPath);
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

	const FString LocalPathMask = FPaths::Combine(FHomePath, TEXT("Rider*"));
	
	IFileManager::Get().FindFiles(RiderPaths, *LocalPathMask, false, true);

	for(const FString& RiderPath: RiderPaths)
	{
		FString FullPath = FPaths::Combine(FHomePath, RiderPath);
		TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(FullPath, false);
		if(InstallInfo.IsSet())
			Result.Add(InstallInfo.GetValue());
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
    FPlatformProcess::ExecProcess(TEXT("/usr/bin/mdfind"), TEXT("\"kMDItemKind == Application\""), &ReturnCode, &OutResults, &OutErrors);
    if (ReturnCode != 0)
		return {};

    TArray<FString> RiderPaths;
	FString TmpString;
	while(OutResults.Split(TEXT("\n"), &TmpString, &OutResults))
	{
		if(TmpString.Contains(TEXT("Rider")))
			RiderPaths.Add(TmpString);
	}
    TArray<FInstallInfo> Result;
    for(const FString& RiderPath: RiderPaths)
    {
        TOptional<FInstallInfo> InstallInfo = FRiderPathLocator::GetInstallInfoFromRiderPath(RiderPath, false);
        if(InstallInfo.IsSet())
            Result.Add(InstallInfo.GetValue());
    }
    return Result;
}

TSet<FInstallInfo> FRiderPathLocator::CollectAllPaths()
{
	TSet<FInstallInfo> InstallInfos;
	InstallInfos.Append(GetInstalledRidersWithMdfind());
	InstallInfos.Append(GetManuallyInstalledRiders());
	InstallInfos.Append(GetInstallInfosFromToolbox(GetToolboxPath(), "Rider*"));
	InstallInfos.Append(GetInstallInfosFromResourceFile());
	return InstallInfos;
}
#endif
