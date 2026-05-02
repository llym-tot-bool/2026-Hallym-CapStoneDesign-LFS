// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Soulslike : ModuleRules
{
	public Soulslike(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Soulslike",
			"Soulslike/Variant_Platforming",
			"Soulslike/Variant_Platforming/Animation",
			"Soulslike/Variant_Combat",
			"Soulslike/Variant_Combat/AI",
			"Soulslike/Variant_Combat/Animation",
			"Soulslike/Variant_Combat/Gameplay",
			"Soulslike/Variant_Combat/Interfaces",
			"Soulslike/Variant_Combat/UI",
			"Soulslike/Variant_SideScrolling",
			"Soulslike/Variant_SideScrolling/AI",
			"Soulslike/Variant_SideScrolling/Gameplay",
			"Soulslike/Variant_SideScrolling/Interfaces",
			"Soulslike/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
