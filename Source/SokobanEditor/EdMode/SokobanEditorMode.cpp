#include "SokobanEditorMode.h"
#include "SokobanEdModeToolkit.h"
#include "SokobanEdModeCommands.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Editor.h"

const FEditorModeID USokobanEditorMode::EM_SokobanEditorMode = TEXT("EM_SokobanEditorMode");

USokobanEditorMode::USokobanEditorMode()
{
	Info = FEditorModeInfo(
		EM_SokobanEditorMode,
		NSLOCTEXT("SokobanEditor", "ModeName", "Sokoban Edit"),
		FSlateIcon(),
		true);
}

void USokobanEditorMode::Enter()
{
	UEdMode::Enter();

	UE_LOG(LogTemp, Log, TEXT("SokobanEditorMode: Enter"));

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

	UE_LOG(LogTemp, Log, TEXT("SokobanEditorMode: Exit"));

	UEdMode::Exit();
}

void USokobanEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FSokobanEdModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> USokobanEditorMode::GetModeCommands() const
{
	return FSokobanEdModeCommands::Get().GetCommands();
}
