// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ISourceCodeAccessor.h"

namespace FRiderPathLocator
{
	struct FInstallInfo;
}

class FRiderSourceCodeAccessor : public ISourceCodeAccessor
{
public:
	void Startup(const FRiderPathLocator::FInstallInfo & Info);

	/** ISourceCodeAccessor implementation */
	virtual void RefreshAvailability() override;
	virtual bool CanAccessSourceCode() const override;
	virtual bool DoesSolutionExist() const override;
	virtual FName GetFName() const override;
	virtual FText GetNameText() const override;
	virtual FText GetDescriptionText() const override;
	virtual bool OpenSolution() override;
	virtual bool OpenSolutionAtPath(const FString& InSolutionPath) override;
	virtual bool OpenFileAtLine(const FString& FullPath, int32 LineNumber, int32 ColumnNumber = 0) override;
	virtual bool OpenSourceFiles(const TArray<FString>& AbsoluteSourcePaths) override;
	virtual bool AddSourceFiles(const TArray<FString>& AbsoluteSourcePaths, const TArray<FString>& AvailableModules) override;
	virtual bool SaveAllOpenDocuments() const override;
	virtual void Tick(const float) override {}
private:

	FName RiderName;

	/**
	 * Is Rider installed on this system?
	 */
	bool bHasRiderInstalled = false;
	/**
	 * The path to the Rider executable.
	 */
	FString ExecutablePath;

#if WITH_EDITOR
	FDelegateHandle BlockEditingInRiderDocumentsDelegateHandle;
#endif
};