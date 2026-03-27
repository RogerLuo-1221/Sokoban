#include "SokobanEdModeCommands.h"

#define LOCTEXT_NAMESPACE "SokobanEditor"

void FSokobanEdModeCommands::RegisterCommands()
{
	UI_COMMAND(PaintTool, "Paint", "Paint tiles on the grid", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(EraseTool, "Erase", "Erase tiles from the grid", EUserInterfaceActionType::RadioButton, FInputChord());
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FSokobanEdModeCommands::GetCommands() const
{
	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Commands;
	TArray<TSharedPtr<FUICommandInfo>> ToolCommands;
	ToolCommands.Add(PaintTool);
	ToolCommands.Add(EraseTool);
	Commands.Add(NAME_Default, ToolCommands);
	return Commands;
}

#undef LOCTEXT_NAMESPACE
