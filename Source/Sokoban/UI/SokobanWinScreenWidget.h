// SokobanWinScreenWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SokobanWinScreenWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextLevelRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMenuRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTestRequested);

UCLASS()
class SOKOBAN_API USokobanWinScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnNextLevelRequested OnNextLevelRequested;

	UPROPERTY(BlueprintAssignable)
	FOnReturnToMenuRequested OnReturnToMenuRequested;

	UPROPERTY(BlueprintAssignable)
	FOnEndTestRequested OnEndTestRequested;

	/** Call before showing to configure button visibility. */
	UFUNCTION(BlueprintCallable, Category = "WinScreen")
	void SetShowNextButton(bool bShow);

	/** Switch to PlayTest mode: hide Next/BackToMenu, show EndTest. */
	UFUNCTION(BlueprintCallable, Category = "WinScreen")
	void SetPlayTestMode(bool bPlayTest);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_NextLevel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_BackToMenu;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_EndTest;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_WinMessage;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	bool bNextButtonVisible = true;
	bool bIsPlayTestMode = false;

	UFUNCTION() void OnNextLevelClicked();
	UFUNCTION() void OnBackToMenuClicked();
	UFUNCTION() void OnEndTestClicked();
};
