// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class NoGreedy : ModuleRules
{
	public NoGreedy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"NoGreedy",
			"NoGreedy/Variant_Platforming",
			"NoGreedy/Variant_Platforming/Animation",
			"NoGreedy/Variant_Combat",
			"NoGreedy/Variant_Combat/AI",
			"NoGreedy/Variant_Combat/Animation",
			"NoGreedy/Variant_Combat/Gameplay",
			"NoGreedy/Variant_Combat/Interfaces",
			"NoGreedy/Variant_Combat/UI",
			"NoGreedy/Variant_SideScrolling",
			"NoGreedy/Variant_SideScrolling/AI",
			"NoGreedy/Variant_SideScrolling/Gameplay",
			"NoGreedy/Variant_SideScrolling/Interfaces",
			"NoGreedy/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
