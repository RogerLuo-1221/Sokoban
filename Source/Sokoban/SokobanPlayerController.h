// SokobanPlayerController.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SokobanTypes.h"
#include "SokobanPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ASokobanGridManager;
class USokobanMainMenuWidget;
class USokobanWinScreenWidget;

UCLASS()
class SOKOBAN_API ASokobanPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASokobanPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// --- Input ---
	void OnMoveInput(const FInputActionValue& Value);
	void OnUndo();
	void OnReset();

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Undo;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Reset;

	UPROPERTY()
	TObjectPtr<ASokobanGridManager> GridManager;

	// --- UI ---
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USokobanMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USokobanWinScreenWidget> WinScreenWidgetClass;

	UPROPERTY()
	TObjectPtr<USokobanMainMenuWidget> MainMenuWidget;

	UPROPERTY()
	TObjectPtr<USokobanWinScreenWidget> WinScreenWidget;

	bool bInMenu = true;

	void ShowMainMenu();
	void HideMainMenu();
	void ShowWinScreen();
	void HideWinScreen();

	UFUNCTION() void OnCategorySelected(ELevelCategory Category);
	UFUNCTION() void OnLevelComplete();
	UFUNCTION() void OnNextLevelRequested();
	UFUNCTION() void OnReturnToMenuRequested();
	UFUNCTION() void OnEndTestRequested();
};