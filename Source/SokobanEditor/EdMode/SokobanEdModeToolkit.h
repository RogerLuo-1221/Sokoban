// SokobanEdModeToolkit.h
#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "Subsystem/EditorGridSubsystem.h"

class FSokobanEdModeToolkit : public FModeToolkit
{
public:
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost,
		TWeakObjectPtr<UEdMode> InOwningMode) override;
	virtual void InvokeUI() override;
	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;

private:
	TSharedPtr<SWidget> PanelWidget;

	// --- Brush ComboBox state ---
	TArray<TSharedPtr<FString>> TypeOptions;
	TSharedPtr<SComboBox<TSharedPtr<FString>>> TypeComboBox;
	TSharedPtr<FString> SelectedTypeItem;

	// Remember the last non-Erase paint mode so we can restore it
	EPaintMode LastPaintMode = EPaintMode::TileType;

	void RebuildTypeOptions(UEditorGridSubsystem* Sub);
	void ApplySelectedType(UEditorGridSubsystem* Sub, TSharedPtr<FString> Item);
};
