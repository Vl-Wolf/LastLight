// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LastLight : ModuleRules
{
	public LastLight(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "PhysicsCore", "Slate" });
	}
}
