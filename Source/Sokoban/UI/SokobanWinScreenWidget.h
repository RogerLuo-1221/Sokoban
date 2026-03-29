// SokobanWinScreenWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SokobanWinScreenWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextLevelRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMenuRequested);

UCLASS()
class SOKOBAN_API USokobanWinScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnNextLevelRequested OnNextLevelRequested;

	UPROPERTY(BlueprintAssignable)
	FOnReturnToMenuRequested OnReturnToMenuRequested;

	/** Call before showing to configure button visibility. */
	UFUNCTION(BlueprintCallable, Category = "WinScreen")
	void SetShowNextButton(bool bShow);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_NextLevel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_BackToMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_WinMessage;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	bool bNextButtonVisible = true;

	UFUNCTION() void OnNextLevelClicked();
	UFUNCTION() void OnBackToMenuClicked();
};
