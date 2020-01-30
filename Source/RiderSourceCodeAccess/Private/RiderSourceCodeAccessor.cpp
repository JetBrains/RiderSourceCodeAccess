// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RiderSourceCodeAccessor.h"

#include "RiderPathLocator.h"

#include "ISourceCodeAccessModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformTime.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#endif
#include "IHotReload.h"
#include "DesktopPlatformModule.h"

#define LOCTEXT_NAMESPACE "RiderSourceCodeAccessor"

DEFINE_LOG_CATEGORY_STATIC(LogRiderAccessor, Log, All);

namespace
{
TOptional<FString> ResolvePathToFile(const FString& FullPath)
{
	FString Path = FullPath;
	if (FPaths::IsRelative(Path))
		Path = FPaths::ConvertRelativePathToFull(Path);

	if (!FPaths::FileExists(Path))
	{
		static const TArray<FString> SubDirs = {
			"/Engine/Source/",
			"/Engine/Plugins/"
		};

		FString EngineRootDir = FPaths::RootDir();
		FPaths::NormalizeFilename(Path);
		int32 Index = INDEX_NONE;
		for (const FString& SubDir : SubDirs)
		{
			Index = Path.Find(SubDir);
			if (Index != INDEX_NONE) break;
		}
		if (Index == INDEX_NONE) return {};

		Path = EngineRootDir.Append(Path.RightChop(Index));
		if (!FPaths::FileExists(Path)) return {};
	}
	return {Path};
} 
}


/** save all open documents in visual studio, when recompiling */
static void OnModuleCompileStarted(bool bIsAsyncCompile)
{
	ISourceCodeAccessModule& SourceCodeAccessModule = FModuleManager::LoadModuleChecked<ISourceCodeAccessModule>("SourceCodeAccess");
	SourceCodeAccessModule.GetAccessor().SaveAllOpenDocuments();
}

void FRiderSourceCodeAccessor::RefreshAvailability()
{
	// If we have an executable path, we certainly have it installed!
	bHasRiderInstalled = !ExecutablePath.IsEmpty() && FPaths::FileExists(ExecutablePath);
}

bool FRiderSourceCodeAccessor::AddSourceFiles(const TArray<FString>& AbsoluteSourcePaths, const TArray<FString>& AvailableModules)
{
	// @todo.Rider Manually add to folders? Or just regenerate
	return false;
}

bool FRiderSourceCodeAccessor::CanAccessSourceCode() const
{
	return bHasRiderInstalled;
}

bool FRiderSourceCodeAccessor::DoesSolutionExist() const
{
	FString SolutionPath;
	return FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath);
}

FText FRiderSourceCodeAccessor::GetDescriptionText() const
{
	return LOCTEXT("RiderDisplayDesc", "Open source code files in Rider");
}

FName FRiderSourceCodeAccessor::GetFName() const
{
	return RiderName;
}

FText FRiderSourceCodeAccessor::GetNameText() const
{
	return FText::FromName(RiderName);
}

bool FRiderSourceCodeAccessor::OpenFileAtLine(const FString& FullPath, int32 LineNumber, int32)
{
	if (!bHasRiderInstalled) return false;
	FString SolutionPath;
	if (!FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath)) return false;
	if (FPaths::IsRelative(SolutionPath))
		SolutionPath = FPaths::ConvertRelativePathToFull(SolutionPath);

	const TOptional<FString> OptionalPath = ResolvePathToFile(FullPath);
	if(!OptionalPath.IsSet()) return false;
	const FString Path = OptionalPath.GetValue();

	const FString Params = FString::Printf(TEXT("\"%s\" --line %d \"%s\""), *SolutionPath, LineNumber, *Path);

	FProcHandle Proc = FPlatformProcess::CreateProc(*ExecutablePath, *Params, true, true, false, nullptr, 0, nullptr, nullptr);
	if (!Proc.IsValid())
	{
		UE_LOG(LogRiderAccessor, Warning, TEXT("Opening file (%s) at a line (%d) failed."), *Path, LineNumber);
		FPlatformProcess::CloseProc(Proc);
		return false;
	}

	return true;
}

bool FRiderSourceCodeAccessor::OpenSolution()
{
	if (!bHasRiderInstalled) return false;

	FString SolutionPath;
	if (FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath))
	{
		const FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*SolutionPath);
		FPlatformProcess::CreateProc(*ExecutablePath, *FullPath, true, true, false, nullptr, 0, nullptr, nullptr);
		return true;
	}

	return false;
}

bool FRiderSourceCodeAccessor::OpenSolutionAtPath(const FString& InSolutionPath)
{
	if (!bHasRiderInstalled) return false;

	FString CorrectSolutionPath = InSolutionPath;
	if (!CorrectSolutionPath.EndsWith(".sln"))
	{
		CorrectSolutionPath += ".sln";
	}
	FProcHandle Proc = FPlatformProcess::CreateProc(*ExecutablePath, *CorrectSolutionPath, true, true, false, nullptr, 0, nullptr, nullptr);
	if (!Proc.IsValid())
	{
		UE_LOG(LogRiderAccessor, Warning, TEXT("Opening the project file (%s) failed."), *CorrectSolutionPath);
		FPlatformProcess::CloseProc(Proc);
		return false;
	}
	return true;
}

bool FRiderSourceCodeAccessor::OpenSourceFiles(const TArray<FString>& AbsoluteSourcePaths)
{
	if (!bHasRiderInstalled) return false;
	FString SolutionPath;
	if (!FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath)) return false;
	if (FPaths::IsRelative(SolutionPath))
		SolutionPath = FPaths::ConvertRelativePathToFull(SolutionPath);

	FString FilePaths = "";
	for (const FString & FullPath : AbsoluteSourcePaths) {
		const TOptional<FString> OptionalPath = ResolvePathToFile(FullPath);
		if(!OptionalPath.IsSet()) return false;
		const FString Path = OptionalPath.GetValue();
		FilePaths += FString::Printf(TEXT("\"%s\" "), *Path);
	}

	const FString Params = FString::Printf(TEXT("\"%s\" %s"), *SolutionPath, *FilePaths);

	FProcHandle Proc = FPlatformProcess::CreateProc(*ExecutablePath, *Params, true, true, false, nullptr, 0, nullptr, nullptr);
	if (!Proc.IsValid())
	{
		UE_LOG(LogRiderAccessor, Warning, TEXT("Opening files (%s) failed."), *FilePaths);
		FPlatformProcess::CloseProc(Proc);
		return false;
	}
	return true;
}

bool FRiderSourceCodeAccessor::SaveAllOpenDocuments() const
{
	return false;
}

void FRiderSourceCodeAccessor::Startup(const FRiderPathLocator::FInstallInfo & Info)
{
	ExecutablePath = Info.Path;
	const FString IsToolboxText = Info.IsToolbox ? TEXT("(toolbox)") : TEXT("(installed)");
	RiderName = *FString::Format(TEXT("Rider {0} {1}"), { Info.Version, IsToolboxText });
#if WITH_EDITOR
	// Setup compilation for saving all VS documents upon compilation start
	BlockEditingInRiderDocumentsDelegateHandle = IHotReloadModule::Get().OnModuleCompilerStarted().AddStatic(&OnModuleCompileStarted);
#endif

	RefreshAvailability();
}

#undef LOCTEXT_NAMESPACE
