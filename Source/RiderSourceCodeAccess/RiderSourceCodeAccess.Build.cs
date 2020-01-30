// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class RiderSourceCodeAccess : ModuleRules
	{
        public RiderSourceCodeAccess(ReadOnlyTargetRules Target) : base(Target)
		{
		    PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "SourceCodeAccess",
                    "DesktopPlatform",
					"Projects"
                }
            );

            if (Target.bBuildEditor)
            {
                PrivateDependencyModuleNames.Add("HotReload");
            }

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PrivateIncludePaths.Add("RiderSourceCodeAccess/Private/RiderPathLocatorWin");
            }
		}
	}
}