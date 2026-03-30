#include "SokobanEditorMode.h"
#include "SokobanEdModeToolkit.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Editor.h"

const FEditorModeID USokobanEditorMode::EM_SokobanEditorMode = TEXT("EM_SokobanEditorMode");

USokobanEditorMode::USokobanEditorMode()
{
	Info = FEditorModeInfo(
		EM_SokobanEditorMode,
		NSLOCTEXT("SokobanEditor", "ModeName", "推箱子编辑"),
		FSlateIcon(),
		true);
}

void USokobanEditorMode::Enter()
{
	UEdMode::Enter();

	if (UEditorGridSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorGridSubsystem>())
	{
		Sub->EnterEditMode();
	} 
}

void USokobanEditorMode::Exit()
{
	if (UEditorGridSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorGridSubsystem>())
	{
		Sub->ExitEditMode();
	}

	UEdMode::Exit();
}

void USokobanEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FSokobanEdModeToolkit);
}

