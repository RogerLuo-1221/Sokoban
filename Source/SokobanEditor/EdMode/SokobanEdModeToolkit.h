// SokobanEdModeToolkit.h
#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FSokobanEdModeToolkit : public FModeToolkit
{
public:
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost,
		TWeakObjectPtr<UEdMode> InOwningMode) override;
	virtual void GetToolPaletteNames(TArray<FName>& PaletteNames) const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual TSharedPtr<SWidget> GetInlineContent() const override;

private:
	TSharedPtr<SWidget> PanelWidget;
};
