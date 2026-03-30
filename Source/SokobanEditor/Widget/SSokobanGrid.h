// SSokobanGrid.h — Slate widget: interactive 2D grid for level editing
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Sokoban/SokobanTypes.h"

class UEditorGridSubsystem;

class SSokobanGrid : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SSokobanGrid)
		: _CellSize(32.0f)
	{}
		SLATE_ARGUMENT(float, CellSize)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// --- SWidget overrides ---
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
		int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FVector2D ComputeDesiredSize(float) const override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	float CellSize = 32.0f;
	bool bIsPainting = false;
	FIntPoint LastPaintedCoord = FIntPoint(-1, -1);

	UEditorGridSubsystem* GetSubsystem() const;
	FIntPoint GetGridCoord(const FGeometry& Geo, const FPointerEvent& Event) const;
	void PaintAtCoord(FIntPoint Coord);
	FLinearColor GetTileColor(ETileType TileType) const;
	FLinearColor GetEntityColor(EEntityType EntityType) const;
};
