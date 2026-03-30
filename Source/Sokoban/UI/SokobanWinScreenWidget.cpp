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
	if (Btn_EndTest)
	{
		Btn_EndTest->OnClicked.AddDynamic(this, &USokobanWinScreenWidget::OnEndTestClicked);
		Btn_EndTest->SetVisibility(ESlateVisibility::Collapsed);
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

void USokobanWinScreenWidget::SetPlayTestMode(bool bPlayTest)
{
	bIsPlayTestMode = bPlayTest;

	if (bPlayTest)
	{
		if (Btn_NextLevel) Btn_NextLevel->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_BackToMenu) Btn_BackToMenu->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_EndTest) Btn_EndTest->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		if (Btn_EndTest) Btn_EndTest->SetVisibility(ESlateVisibility::Collapsed);
		if (Btn_BackToMenu) Btn_BackToMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply USokobanWinScreenWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsPlayTestMode)
	{
		if (InKeyEvent.GetKey() == EKeys::Enter || InKeyEvent.GetKey() == EKeys::SpaceBar
			|| InKeyEvent.GetKey() == EKeys::Escape)
		{
			OnEndTestRequested.Broadcast();
			return FReply::Handled();
		}
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}

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

void USokobanWinScreenWidget::OnEndTestClicked()
{
	OnEndTestRequested.Broadcast();
}
