#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SokobanEdModeStyle.h"

class FSokobanEdModeCommands : public TCommands<FSokobanEdModeCommands>
{
public:
	FSokobanEdModeCommands()
		: TCommands<FSokobanEdModeCommands>(
			TEXT("SokobanEditorMode"),
			NSLOCTEXT("SokobanEditor", "SokobanEditorModeCommands", "Sokoban Editor Mode"),
			NAME_None,
			FSokobanEdModeStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> PaintTool;
	TSharedPtr<FUICommandInfo> EraseTool;

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands() const;
};
