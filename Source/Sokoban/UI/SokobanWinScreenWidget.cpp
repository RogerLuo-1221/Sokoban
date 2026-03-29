// SokobanWinScreenWidget.cpp
#include "SokobanWinScreenWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USokobanWinScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (Btn_NextLevel)
	{
		Btn_NextLevel->OnClicked.AddDynamic(this, &USokobanWinScreenWidget::OnNextLevelClicked);
	}
	if (Btn_BackToMenu)
	{
		Btn_BackToMenu->OnClicked.AddDynamic(this, &USokobanWinScreenWidget::OnBackToMenuClicked);
	}
}

void USokobanWinScreenWidget::SetShowNextButton(bool bShow)
{
	bNextButtonVisible = bShow;
	if (Btn_NextLevel)
	{
		Btn_NextLevel->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

FReply USokobanWinScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::SpaceBar)
	{
		if (bNextButtonVisible)
		{
			OnNextLevelRequested.Broadcast();
			return FReply::Handled();
		}
	}

	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnReturnToMenuRequested.Broadcast();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void USokobanWinScreenWidget::OnNextLevelClicked()
{
	OnNextLevelRequested.Broadcast();
}

void USokobanWinScreenWidget::OnBackToMenuClicked()
{
	OnReturnToMenuRequested.Broadcast();
}
