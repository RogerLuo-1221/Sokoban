// SokobanEdModeToolkit.cpp
#include "SokobanEdModeToolkit.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Widget/SSokobanGrid.h"
#include "Editor.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Styling/CoreStyle.h"

#define LOCTEXT_NAMESPACE "SokobanEditor"

// Active button highlight color
static const FLinearColor ActiveColor(0.2f, 0.5f, 1.0f, 1.0f);   // Blue tint
static const FLinearColor InactiveColor(1.0f, 1.0f, 1.0f, 1.0f); // Default white

// Helper: create a palette button with active-state highlight
static TSharedRef<SWidget> MakePaletteButton(
	const FText& Label,
	TFunction<void()> OnClick,
	TFunction<bool()> IsActive = nullptr)
{
	return SNew(SButton)
		.ButtonColorAndOpacity_Lambda([IsActive]() -> FSlateColor
		{
			return (IsActive && IsActive()) ? FSlateColor(ActiveColor) : FSlateColor(InactiveColor);
		})
		.OnClicked_Lambda([OnClick]()
		{
			if (OnClick) OnClick();
			return FReply::Handled();
		})
		[
			SNew(STextBlock)
			.Text(Label)
			.Justification(ETextJustify::Center)
		];
}

void FSokobanEdModeToolkit::Init(
	const TSharedPtr<IToolkitHost>& InitToolkitHost,
	TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);

	UEditorGridSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorGridSubsystem>();

	// Shared state for grid size spinboxes
	TSharedPtr<int32> GridW = MakeShared<int32>(Sub ? Sub->GetGridWidth() : 8);
	TSharedPtr<int32> GridH = MakeShared<int32>(Sub ? Sub->GetGridHeight() : 8);

	PanelWidget = SNew(SScrollBox)
		+ SScrollBox::Slot().Padding(8)
		[
			SNew(SVerticalBox)

			// ========== Title ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 4)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Title", "Sokoban Level Editor"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
			]

			// ========== Level Name ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2, 0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 4, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("LevelNameLabel", "Level"))
				]
				+ SHorizontalBox::Slot().FillWidth(1)
				[
					SNew(SEditableTextBox)
					.Text_Lambda([Sub]() { return Sub ? FText::FromString(Sub->GetLevelName()) : FText::GetEmpty(); })
					.OnTextCommitted_Lambda([Sub](const FText& NewText, ETextCommit::Type CommitType)
					{
						if (Sub) Sub->SetLevelName(NewText.ToString());
					})
				]
			]

			// ========== Grid Size ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2, 0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 4, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("GridW", "W"))
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 4, 0)
				[
					SNew(SSpinBox<int32>)
					.MinValue(2).MaxValue(32)
					.Value_Lambda([GridW]() { return *GridW; })
					.OnValueChanged_Lambda([GridW](int32 Val) { *GridW = Val; })
				]
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 4, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("GridH", "H"))
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(0, 0, 4, 0)
				[
					SNew(SSpinBox<int32>)
					.MinValue(2).MaxValue(32)
					.Value_Lambda([GridH]() { return *GridH; })
					.OnValueChanged_Lambda([GridH](int32 Val) { *GridH = Val; })
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton)
					.OnClicked_Lambda([Sub, GridW, GridH]()
					{
						if (Sub) Sub->ResizeGrid(*GridW, *GridH);
						return FReply::Handled();
					})
					[
						SNew(STextBlock).Text(LOCTEXT("Resize", "Resize"))
					]
				]
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SSeparator)
			]

			// ========== Tile Palette ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TilePalette", "Tile Palette"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Normal", "Normal"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::TileType); Sub->SetActiveTileType(ETileType::Normal); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::TileType && Sub->GetActiveTileType() == ETileType::Normal; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Wall", "Wall"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::TileType); Sub->SetActiveTileType(ETileType::Wall); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::TileType && Sub->GetActiveTileType() == ETileType::Wall; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Ice", "Ice"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::TileType); Sub->SetActiveTileType(ETileType::Ice); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::TileType && Sub->GetActiveTileType() == ETileType::Ice; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("TargetPad", "Target"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::TileType); Sub->SetActiveTileType(ETileType::TargetPad); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::TileType && Sub->GetActiveTileType() == ETileType::TargetPad; })
				]
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SSeparator)
			]

			// ========== Entity Palette ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("EntityPalette", "Entity Palette"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Player", "Player"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::EntityType); Sub->SetActiveEntityType(EEntityType::Player); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::EntityType && Sub->GetActiveEntityType() == EEntityType::Player; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Box", "Box"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::EntityType); Sub->SetActiveEntityType(EEntityType::Box); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::EntityType && Sub->GetActiveEntityType() == EEntityType::Box; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Erase", "Erase"),
					[Sub]() { if (Sub) { Sub->SetPaintMode(EPaintMode::Erase); } },
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::Erase; })
				]
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SSeparator)
			]

			// ========== 2D Grid ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("GridLabel", "Grid"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]
			+ SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Center).Padding(0, 4)
			[
				SNew(SSokobanGrid).CellSize(32.0f)
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 4)
			[
				SNew(SSeparator)
			]

			// ========== Actions ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Save", "Save"), [Sub]()
					{
						if (Sub) Sub->SaveWithDialog();
					})
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Load", "Load"), [Sub]()
					{
						if (Sub) Sub->LoadWithDialog();
					})
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Clear", "Clear"), [Sub]()
					{
						if (Sub) Sub->ClearGrid();
					})
				]
			]
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 4)
			[
				SNew(SButton)
				.OnClicked_Lambda([Sub]()
				{
					if (Sub) Sub->PlayTest();
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PlayTest", "Play Test"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					.Justification(ETextJustify::Center)
				]
			]
		];

	UE_LOG(LogTemp, Log, TEXT("SokobanEdModeToolkit: Panel built with Slate"));
}

TSharedPtr<SWidget> FSokobanEdModeToolkit::GetInlineContent() const
{
	return PanelWidget;
}

void FSokobanEdModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}

FName FSokobanEdModeToolkit::GetToolkitFName() const
{
	return FName("SokobanEditorMode");
}

FText FSokobanEdModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("ToolkitName", "Sokoban Editor Toolkit");
}

#undef LOCTEXT_NAMESPACE
