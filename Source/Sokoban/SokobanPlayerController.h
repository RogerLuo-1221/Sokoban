// SokobanPlayerController.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SokobanPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ASokobanGridManager;

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
};