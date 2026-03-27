// SSokobanGrid.cpp
#include "SSokobanGrid.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Sokoban/SokobanTypes.h"
#include "Editor.h"
#include "Rendering/DrawElements.h"
#include "Styling/AppStyle.h"
#include "Styling/CoreStyle.h"

void SSokobanGrid::Construct(const FArguments& InArgs)
{
	CellSize = InArgs._CellSize;
	SetVisibility(EVisibility::Visible); // Required: makes widget receive mouse hit-tests
}

UEditorGridSubsystem* SSokobanGrid::GetSubsystem() const
{
	return GEditor ? GEditor->GetEditorSubsystem<UEditorGridSubsystem>() : nullptr;
}

FVector2D SSokobanGrid::ComputeDesiredSize(float) const
{
	if (UEditorGridSubsystem* Sub = GetSubsystem())
	{
		return FVector2D(Sub->GetGridWidth() * CellSize, Sub->GetGridHeight() * CellSize);
	}
	return FVector2D(256, 256);
}

int32 SSokobanGrid::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
	int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	UEditorGridSubsystem* Sub = GetSubsystem();
	if (!Sub) return LayerId;

	const int32 W = Sub->GetGridWidth();
	const int32 H = Sub->GetGridHeight();
	const FSlateBrush* WhiteBrush = FAppStyle::Get().GetBrush("WhiteBrush");
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Bold", 12);
	const float Gap = 1.0f;

	// GridY→screen right, GridX→screen up (Y-flip: X=0 at bottom, X=H-1 at top)
	for (int32 X = 0; X < H; ++X)
	{
		for (int32 Y = 0; Y < W; ++Y)
		{
			FGridCell Cell = Sub->GetCellData(FIntPoint(X, Y));
			FLinearColor Color = GetCellColor(Cell);

			FVector2D CellPos(Y * CellSize, (H - 1 - X) * CellSize);
			FVector2D CellDrawSize(CellSize - Gap, CellSize - Gap);

			// Cell background
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(CellDrawSize, FSlateLayoutTransform(CellPos)),
				WhiteBrush,
				ESlateDrawEffect::None,
				Color);

			// Entity label (P = Player, B = Box)
			if (Cell.EntityType != EEntityType::None)
			{
				FString Label = (Cell.EntityType == EEntityType::Player) ? TEXT("P") : TEXT("B");
				FVector2D TextOffset(Y * CellSize + CellSize * 0.3f, (H - 1 - X) * CellSize + CellSize * 0.15f);

				FSlateDrawElement::MakeText(
					OutDrawElements,
					LayerId + 1,
					AllottedGeometry.ToPaintGeometry(CellDrawSize, FSlateLayoutTransform(TextOffset)),
					Label,
					Font,
					ESlateDrawEffect::None,
					FLinearColor::White);
			}
		}
	}

	return LayerId + 2;
}

// --- Mouse interaction ---

FIntPoint SSokobanGrid::GetGridCoord(const FGeometry& Geo, const FPointerEvent& Event) const
{
	FVector2D LocalPos = Geo.AbsoluteToLocal(Event.GetScreenSpacePosition());
	UEditorGridSubsystem* Sub = GetSubsystem();
	int32 H = Sub ? Sub->GetGridHeight() : 8;  // GridX range (rows)
	// Screen X → GridY (right), Screen Y → GridX (up, flipped)
	int32 GridY = FMath::FloorToInt(LocalPos.X / CellSize);
	int32 GridX = (H - 1) - FMath::FloorToInt(LocalPos.Y / CellSize);
	return FIntPoint(GridX, GridY);
}

void SSokobanGrid::PaintAtCoord(FIntPoint Coord)
{
	if (Coord == LastPaintedCoord) return; // avoid duplicate paints on same cell during drag

	UEditorGridSubsystem* Sub = GetSubsystem();
	if (Sub && Sub->IsValidCoord(Coord))
	{
		Sub->PaintAt(Coord);
		LastPaintedCoord = Coord;
	}
}

FReply SSokobanGrid::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bIsPainting = true;
		LastPaintedCoord = FIntPoint(-1, -1);
		PaintAtCoord(GetGridCoord(MyGeometry, MouseEvent));
		return FReply::Handled().CaptureMouse(SharedThis(this));
	}
	return FReply::Unhandled();
}

FReply SSokobanGrid::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsPainting)
	{
		PaintAtCoord(GetGridCoord(MyGeometry, MouseEvent));
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply SSokobanGrid::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsPainting)
	{
		bIsPainting = false;
		LastPaintedCoord = FIntPoint(-1, -1);
		return FReply::Handled().ReleaseMouseCapture();
	}
	return FReply::Unhandled();
}

// --- Cell color mapping ---

FLinearColor SSokobanGrid::GetCellColor(const FGridCell& Cell) const
{
	// Entity colors take priority
	if (Cell.EntityType == EEntityType::Player)
		return FLinearColor(0.2f, 0.5f, 1.0f);   // Blue
	if (Cell.EntityType == EEntityType::Box)
		return FLinearColor(0.9f, 0.6f, 0.2f);    // Orange

	// Tile type colors
	switch (Cell.TileType)
	{
	case ETileType::Wall:      return FLinearColor(0.35f, 0.35f, 0.35f); // Dark gray
	case ETileType::Ice:       return FLinearColor(0.7f, 0.9f, 1.0f);    // Cyan
	case ETileType::TargetPad: return FLinearColor(1.0f, 0.85f, 0.2f);   // Yellow
	default:                   return FLinearColor(0.88f, 0.88f, 0.88f);  // Light gray
	}
}
