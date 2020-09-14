#include "RiderPathLocator/RiderPathLocator.h"

#include "Dom/JsonObject.h"
#include "Internationalization/Regex.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

TArray<FInstallInfo> FRiderPathLocator::GetInstallInfosFromToolbox(const FString& ToolboxPath, const FString& Pattern)
{
	FString InstallPath = ToolboxPath;
	const FString SettingJsonPath = FPaths::Combine(ToolboxPath, TEXT(".settings.json"));
	if (FPaths::FileExists(SettingJsonPath))
	{
		FString JsonStr;
		FFileHelper::LoadFileToString(JsonStr, *SettingJsonPath);
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			FString InstallLocation;
			if (JsonObject->TryGetStringField(TEXT("install_location"), InstallLocation))
			{
				if (!InstallLocation.IsEmpty())
				{
					InstallPath = InstallLocation;
				}
			}
		}
	}
	
	const FString ToolboxRiderRootPath = FPaths::Combine(InstallPath, TEXT("apps"));
	if(!FPaths::DirectoryExists(ToolboxRiderRootPath)) return {};

	return GetInstallInfos(ToolboxRiderRootPath, Pattern, true);
}

TArray<FInstallInfo> FRiderPathLocator::GetInstallInfos(const FString& ToolboxRiderRootPath, const FString& Pattern, bool IsToolbox)
{
	TArray<FInstallInfo> RiderInstallInfos;
	TArray<FString> RiderPaths;
	IFileManager::Get().FindFilesRecursive(RiderPaths, *ToolboxRiderRootPath, *Pattern, true, true);
	for(const FString& RiderPath: RiderPaths)
	{
		TOptional<FInstallInfo> InstallInfo = GetInstallInfoFromRiderPath(RiderPath, IsToolbox);
		if(InstallInfo.IsSet())
			RiderInstallInfos.Add(InstallInfo.GetValue());
	}
	return RiderInstallInfos;
}