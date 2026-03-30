// SokobanGameMode.cpp
#include "SokobanGameMode.h"
#include "SokobanPlayerController.h"
#include "SokobanPawn.h"

ASokobanGameMode::ASokobanGameMode()
{
	PlayerControllerClass = ASokobanPlayerController::StaticClass();
	DefaultPawnClass = nullptr;  // GridManager spawns the pawn, not GameMode
}