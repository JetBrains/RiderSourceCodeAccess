// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RiderSourceCodeAccessorModule.h"

#include "RiderPathLocator.h"

#include "Modules/ModuleManager.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "RiderSourceCodeAccessor"

IMPLEMENT_MODULE(FRiderSourceCodeAccessModule, RiderSourceCodeAccess);

void FRiderSourceCodeAccessModule::ShutdownModule()
{
	for (auto& RiderSourceCodeAccessor : RiderSourceCodeAccessors)
	{
		// Unbind provider from editor
		IModularFeatures::Get().UnregisterModularFeature(TEXT("SourceCodeAccessor"), &(RiderSourceCodeAccessor.Value.Get()));
	}
}

void FRiderSourceCodeAccessModule::StartupModule()
{
	TArray<FRiderPathLocator::FInstallInfo> InstallInfos = FRiderPathLocator::CollectAllPaths();
	for (const auto & InstallInfo : InstallInfos)
	{
		TSharedRef<FRiderSourceCodeAccessor> RiderSourceCodeAccessor = MakeShareable(new FRiderSourceCodeAccessor());
		RiderSourceCodeAccessor->Startup(InstallInfo);
		IModularFeatures::Get().RegisterModularFeature(TEXT("SourceCodeAccessor"), &RiderSourceCodeAccessor.Get());
		RiderSourceCodeAccessors.Add(RiderSourceCodeAccessor->GetFName(), RiderSourceCodeAccessor);
	}
}

bool FRiderSourceCodeAccessModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE
