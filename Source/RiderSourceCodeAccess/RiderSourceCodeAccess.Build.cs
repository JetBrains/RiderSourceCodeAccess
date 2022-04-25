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
                    "SourceCodeAccess",
                    "DesktopPlatform",
					"Projects",
					"Json",
					"Projects",
					"Slate"
				}
			);

			if (Target.Type == TargetType.Editor)
			{
				PrivateDependencyModuleNames.Add("EditorFramework");
				PrivateDependencyModuleNames.Add("UnrealEd");
				PrivateDependencyModuleNames.Add("GameProjectGeneration");
			}
		}
	}
}