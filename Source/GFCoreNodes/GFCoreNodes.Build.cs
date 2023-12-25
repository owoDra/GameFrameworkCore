// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GFCoreNodes : ModuleRules
{
	public GFCoreNodes(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
            new string[] 
			{
                ModuleDirectory,
                ModuleDirectory + "/GFCoreNodes",
            }
        );	

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"UnrealEd",
				"BlueprintGraph",
				"KismetCompiler",
				"PropertyEditor",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"GFCore",
			}
        );
	}
}
