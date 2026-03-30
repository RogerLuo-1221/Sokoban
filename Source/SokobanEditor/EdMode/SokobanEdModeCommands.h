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
			NSLOCTEXT("SokobanEditor", "SokobanEditorModeCommands", "推箱子编辑模式"),
			NAME_None,
			FSokobanEdModeStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> GetCommands() const;
};

