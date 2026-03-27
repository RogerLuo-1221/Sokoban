using UnrealBuildTool;

public class SokobanEditor : ModuleRules
{
	public SokobanEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			"InputCore",
			"UnrealEd",
			"EditorFramework",
			"EditorSubsystem",
			"InteractiveToolsFramework",
			"EditorInteractiveToolsFramework",
			"Projects",
			"UMG",
			"Blutility",
			"Json",
			"JsonUtilities",
			"DesktopPlatform",
			"Sokoban"
		});
	}
}
