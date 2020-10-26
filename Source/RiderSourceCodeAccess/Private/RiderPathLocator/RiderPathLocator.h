// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

struct FVersion
{
	FVersion() = default;
	~FVersion() = default;
	
	TArray<int32> Versions;

	bool IsInitialized() const { return Versions.Num() != 0; }

	void operator=(const FString& VersionString)
	{
		TArray<FString> Output;
		VersionString.ParseIntoArray(Output, TEXT("."));
		for (const FString& Item : Output)
		{
			Versions.Add(FCString::Atoi(*Item));
		}		
	}

	bool operator<(const FVersion& rhs) const
	{
		TArray<int32>::SizeType LeftSize = Versions.Num();
		TArray<int32>::SizeType RightSize = rhs.Versions.Num();
		const TArray<int32>::SizeType Size = (LeftSize >= RightSize) ? RightSize : LeftSize;
		for(TArray<int32>::SizeType Index = 0; Index < Size; Index++)
		{
			const int32 LeftElement = Versions[Index];
			const int32 RightElement = rhs.Versions[Index];
			if(LeftElement < RightElement) return true;
			if(LeftElement > RightElement) return false;
		}
		return LeftSize < RightSize;
		
	}

	FString ToString() const
	{
		return FString::JoinBy(Versions, TEXT("."), [](int32 Item)-> FString { return FString::FromInt(Item); });
	}
};

struct FInstallInfo
{
	enum class ESupportUproject
	{
		None,
		Beta,
		Release			
	};
	
	FString Path;
	FVersion Version;
	ESupportUproject SupportUprojectState = ESupportUproject::None;
	bool IsToolbox = false;

	FInstallInfo() = default;

	bool operator<(const FInstallInfo& InstallInfo) const
	{
		return Version < InstallInfo.Version;
	}

	bool operator==(const FInstallInfo& InstallInfo) const
	{
		return !(*this < InstallInfo) && !(InstallInfo < *this); 
	}
    
    friend FORCEINLINE uint32 GetTypeHash(const FInstallInfo& InstallInfo)
    {
        return GetTypeHash(InstallInfo.Path);
    }
};

class FRiderPathLocator
{
public:
	// Platform specific implementation
	static TOptional<FInstallInfo> GetInstallInfoFromRiderPath(const FString& Path, bool bIsToolbox);
	static TSet<FInstallInfo> CollectAllPaths();
private:
	static void ParseProductInfoJson(FInstallInfo& Info, const FString& ProductInfoJsonPath);
	static TArray<FInstallInfo> GetInstallInfosFromToolbox(const FString& ToolboxPath, const FString& Pattern);
	static TArray<FInstallInfo> GetInstallInfos(const FString& ToolboxRiderRootPath, const FString& Pattern, bool IsToolbox);
};
