#pragma once

#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
#include "BaseTools/SingleClickTool.h"
#include "TilePaintTool.generated.h"

UCLASS()
class UTilePaintToolBuilder : public UInteractiveToolBuilder
{
	GENERATED_BODY()

public:
	virtual bool CanBuildTool(const FToolBuilderState& SceneState) const override { return true; }
	virtual UInteractiveTool* BuildTool(const FToolBuilderState& SceneState) const override;
};

UCLASS()
class UTilePaintTool : public USingleClickTool
{
	GENERATED_BODY()

public:
	virtual void Setup() override;
	virtual FInputRayHit IsHitByClick(const FInputDeviceRay& ClickPos) override;
	virtual void OnClicked(const FInputDeviceRay& ClickPos) override;

private:
	bool HitTestGrid(const FRay& WorldRay, FIntPoint& OutCoord) const;
};
