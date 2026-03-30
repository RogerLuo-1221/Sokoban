// SokobanEdModeToolkit.cpp
#include "SokobanEdModeToolkit.h"
#include "SokobanEditorMode.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Widget/SSokobanGrid.h"
#include "Editor.h"
#include "EditorModeManager.h"
#include "Containers/Ticker.h"

#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"
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

void FSokobanEdModeToolkit::InvokeUI()
{
	FModeToolkit::InvokeUI();

	// Hook PrimaryTab close — when user closes the toolkit panel,
	// deactivate the mode (deferred to avoid re-entrancy during tab destruction).
	if (TSharedPtr<SDockTab> Tab = PrimaryTab.Pin())
	{
		Tab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateLambda(
			[](TSharedRef<SDockTab>)
			{
				FTSTicker::GetCoreTicker().AddTicker(
					FTickerDelegate::CreateLambda([](float) -> bool
					{
						if (GLevelEditorModeTools().IsModeActive(USokobanEditorMode::EM_SokobanEditorMode))
						{
							GLevelEditorModeTools().DeactivateMode(USokobanEditorMode::EM_SokobanEditorMode);
						}
						return false;
					}));
			}));
	}
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

	// Initialize type options for default category (Tile)
	RebuildTypeOptions(Sub);

	PanelWidget = SNew(SScrollBox)
		+ SScrollBox::Slot().Padding(8)
		[
			SNew(SVerticalBox)

			// ========== Title ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 0, 0, 4)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Title", "推箱子关卡编辑器"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 16))
			]

			// ========== Level Name ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2, 0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 4, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("LevelNameLabel", "关卡名"))
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
					SNew(STextBlock).Text(LOCTEXT("GridW", "宽"))
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
					SNew(STextBlock).Text(LOCTEXT("GridH", "高"))
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
						SNew(STextBlock).Text(LOCTEXT("Resize", "应用"))
					]
				]
			]

			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SSeparator)
			]

			// ========== Brush Section ==========
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 6, 0, 2)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("BrushHeader", "画笔"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
			]

			// Brush Mode: Paint / Erase
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Paint", "绘制"),
					[this, Sub]()
					{
						if (Sub && Sub->GetPaintMode() == EPaintMode::Erase)
						{
							Sub->SetPaintMode(LastPaintMode);
							RebuildTypeOptions(Sub);
						}
					},
					[Sub]() { return Sub && Sub->GetPaintMode() != EPaintMode::Erase; })
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Erase", "擦除"),
					[this, Sub]()
					{
						if (Sub && Sub->GetPaintMode() != EPaintMode::Erase)
						{
							LastPaintMode = Sub->GetPaintMode();
						}
						if (Sub) Sub->SetPaintMode(EPaintMode::Erase);
					},
					[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::Erase; })
				]
			]

			// Paint Options (hidden when Erase mode)
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 4)
			[
				SNew(SVerticalBox)
				.Visibility_Lambda([Sub]()
				{
					return (Sub && Sub->GetPaintMode() != EPaintMode::Erase)
						? EVisibility::Visible : EVisibility::Collapsed;
				})

				// Category: Tile / Entity
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
					[
						MakePaletteButton(LOCTEXT("TileCategory", "地块"),
						[this, Sub]()
						{
							if (Sub)
							{
								Sub->SetPaintMode(EPaintMode::TileType);
								RebuildTypeOptions(Sub);
							}
						},
						[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::TileType; })
					]
					+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
					[
						MakePaletteButton(LOCTEXT("EntityCategory", "实体"),
						[this, Sub]()
						{
							if (Sub)
							{
								Sub->SetPaintMode(EPaintMode::EntityType);
								RebuildTypeOptions(Sub);
							}
						},
						[Sub]() { return Sub && Sub->GetPaintMode() == EPaintMode::EntityType; })
					]
				]

				// Type dropdown
				+ SVerticalBox::Slot().AutoHeight().Padding(4, 4)
				[
					SAssignNew(TypeComboBox, SComboBox<TSharedPtr<FString>>)
					.OptionsSource(&TypeOptions)
					.OnSelectionChanged_Lambda([this, Sub](TSharedPtr<FString> Item, ESelectInfo::Type)
					{
						if (Item.IsValid())
						{
							SelectedTypeItem = Item;
							ApplySelectedType(Sub, Item);
						}
					})
					.OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) -> TSharedRef<SWidget>
					{
						return SNew(STextBlock).Text(FText::FromString(*Item));
					})
					[
						SNew(STextBlock)
						.Text_Lambda([this]()
						{
							return SelectedTypeItem.IsValid()
								? FText::FromString(*SelectedTypeItem)
								: FText::GetEmpty();
						})
					]
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
				.Text(LOCTEXT("GridLabel", "网格"))
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
					MakePaletteButton(LOCTEXT("Undo", "撤销"), [Sub]()
					{
						if (Sub) Sub->Undo();
					})
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Save", "保存"), [Sub]()
					{
						if (Sub) Sub->SaveWithDialog();
					})
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Load", "加载"), [Sub]()
					{
						if (Sub) Sub->LoadWithDialog();
					})
				]
				+ SHorizontalBox::Slot().FillWidth(1).Padding(2)
				[
					MakePaletteButton(LOCTEXT("Clear", "清空"), [Sub]()
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
					.Text(LOCTEXT("PlayTest", "试玩"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
					.Justification(ETextJustify::Center)
				]
			]
		];
}

void FSokobanEdModeToolkit::RebuildTypeOptions(UEditorGridSubsystem* Sub)
{
	TypeOptions.Empty();

	if (!Sub) return;

	if (Sub->GetPaintMode() != EPaintMode::EntityType)
	{
		// Tile types (Normal excluded — use Erase to reset to Normal)
		TypeOptions.Add(MakeShared<FString>(TEXT("墙体")));      // index 0 → Wall(1)
		TypeOptions.Add(MakeShared<FString>(TEXT("冰面")));      // index 1 → Ice(2)
		TypeOptions.Add(MakeShared<FString>(TEXT("目标点")));    // index 2 → TargetPad(3)

		// Select the one matching current active tile
		int32 Index = (int32)Sub->GetActiveTileType() - 1; // Wall=1→idx0, Ice=2→idx1, TargetPad=3→idx2
		if (TypeOptions.IsValidIndex(Index))
		{
			SelectedTypeItem = TypeOptions[Index];
		}
		else
		{
			SelectedTypeItem = TypeOptions[0];
		}
	}
	else
	{
		// Entity types: order matches EEntityType (Player=1, Box=2)
		TypeOptions.Add(MakeShared<FString>(TEXT("玩家")));     // index 0 → enum 1
		TypeOptions.Add(MakeShared<FString>(TEXT("箱子")));     // index 1 → enum 2

		int32 Index = (int32)Sub->GetActiveEntityType() - 1;
		if (TypeOptions.IsValidIndex(Index))
		{
			SelectedTypeItem = TypeOptions[Index];
		}
		else
		{
			SelectedTypeItem = TypeOptions[0];
		}
	}

	if (TypeComboBox.IsValid())
	{
		TypeComboBox->RefreshOptions();
		TypeComboBox->SetSelectedItem(SelectedTypeItem);
	}
}

void FSokobanEdModeToolkit::ApplySelectedType(UEditorGridSubsystem* Sub, TSharedPtr<FString> Item)
{
	if (!Sub || !Item.IsValid()) return;

	const FString& TypeName = *Item;

	if (Sub->GetPaintMode() == EPaintMode::TileType)
	{
		if (TypeName == TEXT("墙体"))           Sub->SetActiveTileType(ETileType::Wall);
		else if (TypeName == TEXT("冰面"))     Sub->SetActiveTileType(ETileType::Ice);
		else if (TypeName == TEXT("目标点"))  Sub->SetActiveTileType(ETileType::TargetPad);
	}
	else if (Sub->GetPaintMode() == EPaintMode::EntityType)
	{
		if (TypeName == TEXT("玩家"))  Sub->SetActiveEntityType(EEntityType::Player);
		else if (TypeName == TEXT("箱子")) Sub->SetActiveEntityType(EEntityType::Box);
	}
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
	return LOCTEXT("ToolkitName", "推箱子编辑器工具面板");
}

#undef LOCTEXT_NAMESPACE

