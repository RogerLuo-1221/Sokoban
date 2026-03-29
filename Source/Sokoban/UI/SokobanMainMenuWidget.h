// SokobanMainMenuWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sokoban/SokobanTypes.h"
#include "SokobanMainMenuWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelected, ELevelCategory, Category);

UCLASS()
class SOKOBAN_API USokobanMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCategorySelected OnCategorySelected;

protected:
	UFUNCTION(BlueprintCallable, Category = "Menu")
	void SelectCategory(ELevelCategory Category);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Tutorial;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Beginner;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Intermediate;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> Btn_Advanced;

	virtual void NativeConstruct() override;

private:
	UFUNCTION() void OnTutorialClicked();
	UFUNCTION() void OnBeginnerClicked();
	UFUNCTION() void OnIntermediateClicked();
	UFUNCTION() void OnAdvancedClicked();
};
