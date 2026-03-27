#include "TilePaintTool.h"
#include "InteractiveToolManager.h"
#include "Subsystem/EditorGridSubsystem.h"
#include "Editor.h"

UInteractiveTool* UTilePaintToolBuilder::BuildTool(const FToolBuilderState& SceneState) const
{
	return NewObject<UTilePaintTool>(SceneState.ToolManager);
}

void UTilePaintTool::Setup()
{
	USingleClickTool::Setup();
	UE_LOG(LogTemp, Log, TEXT("TilePaintTool: Setup"));
}

FInputRayHit UTilePaintTool::IsHitByClick(const FInputDeviceRay& ClickPos)
{
	FIntPoint GridCoord;
	if (HitTestGrid(ClickPos.WorldRay, GridCoord))
	{
		return FInputRayHit(0.0);
	}
	return FInputRayHit();
}

void UTilePaintTool::OnClicked(const FInputDeviceRay& ClickPos)
{
	FIntPoint GridCoord;
	if (HitTestGrid(ClickPos.WorldRay, GridCoord))
	{
		if (UEditorGridSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorGridSubsystem>())
		{
			Sub->PaintAt(GridCoord);
		}
	}
}

bool UTilePaintTool::HitTestGrid(const FRay& WorldRay, FIntPoint& OutCoord) const
{
	// Ray-plane intersection at Z=0
	if (FMath::IsNearlyZero(WorldRay.Direction.Z))
	{
		return false;
	}

	float T = -WorldRay.Origin.Z / WorldRay.Direction.Z;
	if (T < 0.f)
	{
		return false;
	}

	FVector HitPoint = WorldRay.Origin + WorldRay.Direction * T;

	UEditorGridSubsystem* Sub = GEditor->GetEditorSubsystem<UEditorGridSubsystem>();
	if (!Sub)
	{
		return false;
	}

	float CellSize = Sub->GetCellSize();
	OutCoord = FIntPoint(
		FMath::FloorToInt(HitPoint.X / CellSize),
		FMath::FloorToInt(HitPoint.Y / CellSize)
	);

	return Sub->IsValidCoord(OutCoord);
}
