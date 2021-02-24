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

void FRiderPathLocator::ParseProductInfoJson(FInstallInfo& Info, const FString& ProductInfoJsonPath)
{
	FString JsonStr;
	FFileHelper::LoadFileToString(JsonStr, *ProductInfoJsonPath);
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		FString VersionString;
		JsonObject->TryGetStringField(TEXT("buildNumber"), VersionString);
		Info.Version = VersionString;
		const TArray< TSharedPtr<FJsonValue> >* CustomProperties;
		if(!JsonObject->TryGetArrayField(TEXT("customProperties"), CustomProperties)) return;

		for (const TSharedPtr<FJsonValue>& CustomProperty : *CustomProperties)
		{
			const TSharedPtr<FJsonObject> Item = CustomProperty->AsObject();
			if(!Item.IsValid()) continue;
			
			FString SupportUprojectStateKey;
			const bool bIsValidKey = Item->TryGetStringField(TEXT("key"), SupportUprojectStateKey);
			if(!bIsValidKey) continue;
			if(!SupportUprojectStateKey.Equals(TEXT("SupportUproject"))) continue;

			FString SupportUprojectStateValue;
			const bool bIsValidValue  = Item->TryGetStringField(TEXT("value"), SupportUprojectStateValue);
			if(!bIsValidValue) continue;
			
			if(SupportUprojectStateValue.Equals(TEXT("Beta"))) Info.SupportUprojectState = FInstallInfo::ESupportUproject::Beta;
			if(SupportUprojectStateValue.Equals(TEXT("Release"))) Info.SupportUprojectState = FInstallInfo::ESupportUproject::Release;
		}
	}
}
