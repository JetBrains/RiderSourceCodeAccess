// Copyright Epic Games, Inc. All Rights Reserved.

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
	TArray<FRiderPathLocator::FInstallInfo> InstallInfos = FRiderPathLocator::CollectAllPaths().Array();
	InstallInfos.Sort();
	for (const FRiderPathLocator::FInstallInfo & InstallInfo : InstallInfos)
	{
		TSharedRef<FRiderSourceCodeAccessor> RiderSourceCodeAccessor = MakeShareable(new FRiderSourceCodeAccessor());
		RiderSourceCodeAccessor->Startup(InstallInfo);
		IModularFeatures::Get().RegisterModularFeature(TEXT("SourceCodeAccessor"), &RiderSourceCodeAccessor.Get());
		RiderSourceCodeAccessors.Add(RiderSourceCodeAccessor->GetFName(), RiderSourceCodeAccessor);
	}
	if(InstallInfos.Num() != 0)
	{
		TSharedRef<FRiderSourceCodeAccessor> RiderSourceCodeAccessor = MakeShareable(new FRiderSourceCodeAccessor());
		RiderSourceCodeAccessor->Startup(InstallInfos.Last(), FRiderSourceCodeAccessor::ACCESS_TYPE::AGGREGATE);
		IModularFeatures::Get().RegisterModularFeature(TEXT("SourceCodeAccessor"), &RiderSourceCodeAccessor.Get());
		RiderSourceCodeAccessors.Add("Rider", RiderSourceCodeAccessor);		
	}
}

bool FRiderSourceCodeAccessModule::SupportsDynamicReloading()
{
	return true;
}

#undef LOCTEXT_NAMESPACE
