// SokobanGameMode.cpp
#include "SokobanGameMode.h"
#include "SokobanPlayerController.h"
#include "SokobanPawn.h"

ASokobanGameMode::ASokobanGameMode()
{
	PlayerControllerClass = ASokobanPlayerController::StaticClass();
	DefaultPawnClass = nullptr;  // GridManager spawns the pawn, not GameMode
}

void ASokobanGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Camera setup: find the player controller, set fixed view
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		// Disable default pawn possession since GridManager handles it
		// Camera will be set up separately
	}
}