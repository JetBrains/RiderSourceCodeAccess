#include "RiderPathLocator.h"

#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Windows/WindowsPlatformMisc.h"
#include "Runtime/Launch/Resources/Version.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>

namespace FRiderPathLocator
{

	struct FCollectFoldersVisitor : IPlatformFile::FDirectoryVisitor
	{
		bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory)
				Folders.Add(FilenameOrDirectory);
			return true;
		}
		TArray<FString> Folders;
	};

	static TArray<FInstallInfo> CollectPathsFromToolbox(const FString& IDEName)
	{
		TArray<FInstallInfo> RiderPaths;

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 20
		TCHAR CAppDataLocalPath[4096];
		FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA"), CAppDataLocalPath, ARRAY_COUNT(AppDataLocalPath));
		const FString FAppDataLocalPath = CAppDataLocalPath;
#else
		const FString FAppDataLocalPath = FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA"));
#endif
		const FString ToolboxRiderRootPath = FPaths::Combine(*FAppDataLocalPath, TEXT("JetBrains\\Toolbox\\apps"), *IDEName);

		FCollectFoldersVisitor ChannelDirs;
		FCollectFoldersVisitor RiderDirs;
		IFileManager::Get().IterateDirectory(*ToolboxRiderRootPath, ChannelDirs);
		for (const auto & ChannelDir : ChannelDirs.Folders)
		{
			IFileManager::Get().IterateDirectory(*ChannelDir, RiderDirs);
		}

		for (const auto & RiderDir : RiderDirs.Folders)
		{
			const FString RiderExePath = FPaths::Combine(RiderDir, TEXT("bin\\rider64.exe"));
			const FString RiderCppPluginPath = FPaths::Combine(RiderDir, TEXT("plugins\\rider-cpp"));
			if (FPaths::FileExists(RiderExePath) && FPaths::DirectoryExists(RiderCppPluginPath))
			{
				FInstallInfo Info;
				Info.Path = RiderExePath;
				Info.IsToolbox = true;
				const FString BuildTxtPath = FPaths::Combine(RiderDir, TEXT("build.txt"));
				if (FPaths::FileExists(BuildTxtPath))
				{
					FFileHelper::LoadFileToString(Info.Version, *BuildTxtPath);
				}
				else
				{
					Info.Version = FPaths::GetBaseFilename(RiderDir);
				}
				RiderPaths.Add(Info);
			}
		}

		return RiderPaths;
	}

	static bool EnumerateRegistryKeys(HKEY Key, TArray<FString> &OutNames)
	{
		for (DWORD Index = 0;; Index++)
		{
			TCHAR KeyName[256];
			DWORD KeyNameLength = sizeof(KeyName) / sizeof(KeyName[0]);

			const LONG Result = RegEnumKeyEx(Key, Index, KeyName, &KeyNameLength, NULL, NULL, NULL, NULL);
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

			const LONG Result = RegEnumValue(Key, Index, ValueName, &ValueNameLength, NULL, NULL, NULL, NULL);
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
		const ULONG Result = RegQueryValueExW(Key, *ValueName, 0, NULL, (LPBYTE)Buffer, &BufferSize);
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
				for (const auto& key : Keys)
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
						FString ExePath = FPaths::Combine(InstallLocation, TEXT("bin\\rider64.exe"));
						if (FPaths::FileExists(ExePath))
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
		InstallInfos.Append(CollectPathsFromToolbox(TEXT("Rider")));
		InstallInfos.Append(CollectPathsFromToolbox(TEXT("RiderCpp")));
		InstallInfos.Append(CollectPathsFromRegistry(TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")));
		InstallInfos.Append(CollectPathsFromRegistry(TEXT("SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")));
		return InstallInfos;
	}
}

#include "Windows/HideWindowsPlatformTypes.h"
