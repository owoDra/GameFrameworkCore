// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GFCore : ModuleRules
{
	public GFCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GFCore",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "ModularGameplay",
                "GameplayTags",
                "GameFeatures",
                "DeveloperSettings",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "NetCore",
                "AIModule",
            }
        );
    }
}
