#include "SokobanEdModeStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FSokobanEdModeStyle::StyleInstance = nullptr;

FName FSokobanEdModeStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SokobanEdModeStyle"));
	return StyleSetName;
}

void FSokobanEdModeStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSokobanEdModeStyle::Shutdown()
{
	if (StyleInstance.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
		StyleInstance.Reset();
	}
}

const ISlateStyle& FSokobanEdModeStyle::Get()
{
	return *StyleInstance;
}
