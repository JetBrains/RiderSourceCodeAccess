// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ISourceCodeAccessModule.h"

class FRiderSourceCodeAccessModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;
private:
	void GenerateSlnAccessors(const TArray<struct FInstallInfo>& InstallInfos);
	void GenerateUprojectAccessors(const TArray<struct FInstallInfo>& InstallInfos);
	TMap<FName, TSharedRef<ISourceCodeAccessor>> RiderSourceCodeAccessors;
};
