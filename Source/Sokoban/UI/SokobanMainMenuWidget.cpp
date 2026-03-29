// SokobanMainMenuWidget.cpp
#include "SokobanMainMenuWidget.h"
#include "Components/Button.h"

void USokobanMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Tutorial)
	{
		Btn_Tutorial->OnClicked.AddDynamic(this, &USokobanMainMenuWidget::OnTutorialClicked);
	}
	if (Btn_Beginner)
	{
		Btn_Beginner->OnClicked.AddDynamic(this, &USokobanMainMenuWidget::OnBeginnerClicked);
	}
	if (Btn_Intermediate)
	{
		Btn_Intermediate->OnClicked.AddDynamic(this, &USokobanMainMenuWidget::OnIntermediateClicked);
	}
	if (Btn_Advanced)
	{
		Btn_Advanced->OnClicked.AddDynamic(this, &USokobanMainMenuWidget::OnAdvancedClicked);
	}
}

void USokobanMainMenuWidget::SelectCategory(ELevelCategory Category)
{
	OnCategorySelected.Broadcast(Category);
}

void USokobanMainMenuWidget::OnTutorialClicked()
{
	SelectCategory(ELevelCategory::Tutorial);
}

void USokobanMainMenuWidget::OnBeginnerClicked()
{
	SelectCategory(ELevelCategory::Beginner);
}

void USokobanMainMenuWidget::OnIntermediateClicked()
{
	SelectCategory(ELevelCategory::Intermediate);
}

void USokobanMainMenuWidget::OnAdvancedClicked()
{
	SelectCategory(ELevelCategory::Advanced);
}
