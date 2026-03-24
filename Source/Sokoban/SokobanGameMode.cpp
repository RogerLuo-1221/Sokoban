// Copyright Epic Games, Inc. All Rights Reserved.

#include "SokobanGameMode.h"
#include "SokobanCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASokobanGameMode::ASokobanGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
