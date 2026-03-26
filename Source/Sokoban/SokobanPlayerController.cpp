// SokobanPlayerController.cpp
#include "SokobanPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SokobanGridManager.h"
#include "Kismet/GameplayStatics.h"

ASokobanPlayerController::ASokobanPlayerController()
{
	bShowMouseCursor = false;
}

void ASokobanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add mapping context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	// Find GridManager in level
	GridManager = Cast<ASokobanGridManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ASokobanGridManager::StaticClass()));
}

void ASokobanPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC) return;

	EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ThisClass::OnMoveInput);
	EIC->BindAction(IA_Undo, ETriggerEvent::Triggered, this, &ThisClass::OnUndo);
	EIC->BindAction(IA_Reset, ETriggerEvent::Triggered, this, &ThisClass::OnReset);
}

void ASokobanPlayerController::OnMoveInput(const FInputActionValue& Value)
{
	if (!GridManager) return;
	if (GridManager->IsMoving()) return;

	FVector2D Input = Value.Get<FVector2D>();

	EGridDirection Dir;
	if (FMath::Abs(Input.X) > FMath::Abs(Input.Y))
		Dir = Input.X > 0 ? EGridDirection::East : EGridDirection::West;
	else
		Dir = Input.Y > 0 ? EGridDirection::North : EGridDirection::South;

	GridManager->TryMove(Dir);
}

void ASokobanPlayerController::OnUndo()
{
	if (!GridManager) return;
	if (GridManager->IsMoving()) return;
	GridManager->Undo();
}

void ASokobanPlayerController::OnReset()
{
	if (!GridManager) return;
	GridManager->ResetLevel();
}