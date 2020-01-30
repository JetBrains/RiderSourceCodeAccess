// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISourceCodeAccessModule.h"
#include "RiderSourceCodeAccessor.h"

class FRiderSourceCodeAccessModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override;
private:
	TMap<FName, TSharedRef<FRiderSourceCodeAccessor>> RiderSourceCodeAccessors;
};
