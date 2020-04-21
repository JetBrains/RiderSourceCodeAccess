// Copyright Epic Games, Inc. All Rights Reserved.

#include "RiderPathLocator.h"

#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/FileManager.h"
#include "Internationalization/Regex.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "Windows/WindowsPlatformMisc.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>
#include "Windows/HideWindowsPlatformTypes.h"

namespace FRiderPathLocator
{
	static TArray<FInstallInfo> CollectPathsFromToolbox()
	{
		FString ToolboxBinPath;

		if (!FWindowsPlatformMisc::QueryRegKey(HKEY_CURRENT_USER, TEXT("Software\\JetBrains\\Toolbox\\"), TEXT(""), ToolboxBinPath)) return {};

		FPaths::NormalizeDirectoryName(ToolboxBinPath);
		const FString PatternString(TEXT("(.*)/bin"));
		const FRegexPattern Pattern(PatternString);
		FRegexMatcher ToolboxPathMatcher(Pattern, ToolboxBinPath);
		if (!ToolboxPathMatcher.FindNext()) return {};

		const FString ToolboxPath = ToolboxPathMatcher.GetCaptureGroup(1);
		FString InstallPath = ToolboxPath;
		const FString SettingJsonPath = FPaths::Combine(ToolboxPath, FString(".settings.json"));
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
		if(!FPaths::DirectoryExists(InstallPath)) return {};
		
		const FString ToolboxRiderRootPath = FPaths::Combine(InstallPath, FString("apps"));
		if(!FPaths::DirectoryExists(ToolboxRiderRootPath)) return {};

		TArray<FInstallInfo> RiderInstallInfos;
		TArray<FString> RiderPaths;
		IFileManager::Get().FindFilesRecursive(RiderPaths, *ToolboxRiderRootPath, TEXT("rider64.exe"), true, false);
		for(const FString& RiderPath: RiderPaths)
		{
			FRegexMatcher RiderPathMatcher(Pattern, RiderPath);
			if (!RiderPathMatcher.FindNext()) continue;

			const FString RiderDir = RiderPathMatcher.GetCaptureGroup(1);
			const FString RiderCppPluginPath = FPaths::Combine(RiderDir, TEXT("plugins\\rider-cpp"));
			if (FPaths::FileExists(RiderPath) && FPaths::DirectoryExists(RiderCppPluginPath))
			{
				FInstallInfo Info;
				Info.Path = RiderPath;
				Info.IsToolbox = true;
				const FString ProductInfoJsonPath = FPaths::Combine(RiderDir, TEXT("product-info.json"));
				if (FPaths::FileExists(ProductInfoJsonPath))
				{
					FString JsonStr;
					FFileHelper::LoadFileToString(JsonStr, *ProductInfoJsonPath);
					TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
					TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
					if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
					{
						JsonObject->TryGetStringField(TEXT("buildNumber"), Info.Version);
					}
				}
				if(Info.Version.IsEmpty())
				{
					Info.Version = FPaths::GetBaseFilename(RiderDir);
				}
				RiderInstallInfos.Add(Info);
			}
		}

		return RiderInstallInfos;
	}

	static bool EnumerateRegistryKeys(HKEY Key, TArray<FString> &OutNames)
	{
		for (DWORD Index = 0;; Index++)
		{
			TCHAR KeyName[256];
			DWORD KeyNameLength = sizeof(KeyName) / sizeof(KeyName[0]);

			const LONG Result = RegEnumKeyEx(Key, Index, KeyName, &KeyNameLength, nullptr, nullptr, nullptr, nullptr);
			if (Result == ERROR_NO_MORE_ITEMS)
			{
				break;
			}

			if (Result != ERROR_SUCCESS)
			{
				return false;
			}

			OutNames.Add(KeyName);
		}
		return true;
	}

	static bool EnumerateRegistryValues(HKEY Key, TArray<FString> &OutNames)
	{
		for (DWORD Index = 0;; Index++)
		{
			// Query the value
			wchar_t ValueName[256];
			DWORD ValueNameLength = sizeof(ValueName) / sizeof(ValueName[0]);

			const LONG Result = RegEnumValue(Key, Index, ValueName, &ValueNameLength, nullptr, nullptr, nullptr, nullptr);
			if (Result == ERROR_NO_MORE_ITEMS)
			{
				break;
			}
			
			if (Result != ERROR_SUCCESS)
			{
				return false;
			}

			// Add it to the array
			OutNames.Add(ValueName);
		}
		return true;
	}

	static LONG GetStringRegKey(const HKEY Key, const FString& ValueName, FString& Value)
	{
		WCHAR Buffer[512];
		DWORD BufferSize = sizeof(Buffer);
		const ULONG Result = RegQueryValueExW(Key, *ValueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(Buffer), &BufferSize);
		if (Result == ERROR_SUCCESS)
		{
			Value = Buffer;
		}
		return Result;
	}

	static TArray<FInstallInfo> CollectPathsFromRegistry(const FString& RegistryKey)
	{
		TArray<FInstallInfo> InstallInfos;
		HKEY Key;
		const LONG Result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, *RegistryKey, 0, KEY_READ, &Key);
		if (Result == ERROR_SUCCESS)
		{
			TArray<FString> Keys;
			if (EnumerateRegistryKeys(Key, Keys))
			{
				for (const FString& key : Keys)
				{
					if (!key.Contains(TEXT("Rider"))) continue;

					HKEY SubKey;
					const LONG SubResult = RegOpenKeyEx(Key, *key, 0, KEY_READ, &SubKey);
					if (SubResult != ERROR_SUCCESS) continue;

					TArray<FString> Values;
					if (!EnumerateRegistryValues(SubKey, Values)) continue;

					for (const auto& Value : Values)
					{
						if (Value != TEXT("InstallLocation")) continue;
						FString InstallLocation;
						if (GetStringRegKey(SubKey, Value, InstallLocation) != ERROR_SUCCESS) continue;
						const FString ExePath = FPaths::Combine(InstallLocation, TEXT("bin\\rider64.exe"));
						const FString RiderCppPluginPath = FPaths::Combine(InstallLocation, TEXT("plugins\\rider-cpp"));
						if (FPaths::FileExists(ExePath) && FPaths::DirectoryExists(RiderCppPluginPath))
						{
							FInstallInfo Info;
							Info.IsToolbox = false;
							Info.Path = ExePath;
							GetStringRegKey(SubKey, TEXT("DisplayVersion"), Info.Version);
							InstallInfos.Add(Info);
						}
					}

				}
			}
		}

		return InstallInfos;
	}

	TArray<FInstallInfo> CollectAllPaths()
	{
		TArray<FInstallInfo> InstallInfos;
		InstallInfos.Append(CollectPathsFromToolbox());
		InstallInfos.Append(CollectPathsFromRegistry(TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")));
		InstallInfos.Append(CollectPathsFromRegistry(TEXT("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")));
		return InstallInfos;
	}
}

