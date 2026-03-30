#include "SokobanEdModeCommands.h"

#define LOCTEXT_NAMESPACE "SokobanEditor"

void FSokobanEdModeCommands::RegisterCommands()
{
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FSokobanEdModeCommands::GetCommands() const
{
	return TMap<FName, TArray<TSharedPtr<FUICommandInfo>>>();
}

#undef LOCTEXT_NAMESPACE
