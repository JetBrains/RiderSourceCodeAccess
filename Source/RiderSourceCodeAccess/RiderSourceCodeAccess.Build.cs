// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class RiderSourceCodeAccess : ModuleRules
	{
        public RiderSourceCodeAccess(ReadOnlyTargetRules Target) : base(Target)
        {
	        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		    PrivateDependencyModuleNames.AddRange(
                new []
                {
                    "Core",
                    "UnrealEd",
                    "GameProjectGeneration",
                    "SourceCodeAccess",
                    "DesktopPlatform",
					"Projects",
					"Json"
                }
            );

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PrivateIncludePaths.Add("RiderSourceCodeAccess/Private/RiderPathLocatorWin");
            }
		}
	}
}