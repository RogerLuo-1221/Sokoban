// SokobanPlayerController.cpp
#include "SokobanPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "SokobanGridManager.h"
#include "UI/SokobanMainMenuWidget.h"
#include "UI/SokobanWinScreenWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

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

	// Bind level complete delegate
	if (GridManager)
	{
		GridManager->OnLevelComplete.AddDynamic(this, &ASokobanPlayerController::OnLevelComplete);
	}

	// Create UI widgets
	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<USokobanMainMenuWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport(10);
			MainMenuWidget->OnCategorySelected.AddDynamic(this, &ASokobanPlayerController::OnCategorySelected);
		}
	}

	if (WinScreenWidgetClass)
	{
		WinScreenWidget = CreateWidget<USokobanWinScreenWidget>(this, WinScreenWidgetClass);
		if (WinScreenWidget)
		{
			WinScreenWidget->AddToViewport(20);
			WinScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
			WinScreenWidget->OnNextLevelRequested.AddDynamic(this, &ASokobanPlayerController::OnNextLevelRequested);
			WinScreenWidget->OnReturnToMenuRequested.AddDynamic(this, &ASokobanPlayerController::OnReturnToMenuRequested);
		}
	}

	// Show main menu on start (unless PlayTest mode)
	bool bIsPlayTest = GridManager && GridManager->IsPlayTestMode();
	if (!bIsPlayTest && MainMenuWidget)
	{
		// Normal mode: show main menu, wait for category selection
		ShowMainMenu();
	}
	else
	{
		// PlayTest mode or no UI configured: go straight to game
		bInMenu = false;
		SetInputMode(FInputModeGameOnly());
		if (MainMenuWidget)
		{
			MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
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

// ============================================================
// Input Handlers
// ============================================================

void ASokobanPlayerController::OnMoveInput(const FInputActionValue& Value)
{
	if (!GridManager) return;
	if (bInMenu) return;
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
	if (bInMenu) return;
	if (GridManager->IsMoving()) return;
	GridManager->Undo();
}

void ASokobanPlayerController::OnReset()
{
	if (!GridManager) return;
	if (bInMenu) return;
	GridManager->ResetLevel();
}

// ============================================================
// UI Management
// ============================================================

void ASokobanPlayerController::ShowMainMenu()
{
	bInMenu = true;
	if (MainMenuWidget)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}

void ASokobanPlayerController::HideMainMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ASokobanPlayerController::ShowWinScreen()
{
	if (WinScreenWidget)
	{
		WinScreenWidget->SetVisibility(ESlateVisibility::Visible);
		WinScreenWidget->SetFocus();
	}
	bShowMouseCursor = true;
	SetInputMode(FInputModeGameAndUI());
}

void ASokobanPlayerController::HideWinScreen()
{
	if (WinScreenWidget)
	{
		WinScreenWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());
}

// ============================================================
// UI Event Handlers
// ============================================================

void ASokobanPlayerController::OnCategorySelected(ELevelCategory Category)
{
	HideMainMenu();
	bInMenu = false;
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	if (GridManager)
	{
		GridManager->LoadCategory(Category);
	}
}

void ASokobanPlayerController::OnLevelComplete()
{
	if (!GridManager) return;

	bool bIsLast = GridManager->IsLastLevel();

	if (WinScreenWidget)
	{
		WinScreenWidget->SetShowNextButton(!bIsLast);
		ShowWinScreen();
	}
}

void ASokobanPlayerController::OnNextLevelRequested()
{
	HideWinScreen();

	if (GridManager)
	{
		GridManager->LoadNextLevel();
	}
}

void ASokobanPlayerController::OnReturnToMenuRequested()
{
	HideWinScreen();

	if (GridManager)
	{
		GridManager->ClearLevel();
	}

	ShowMainMenu();
}
