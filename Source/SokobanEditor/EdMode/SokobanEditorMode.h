#pragma once

#include "CoreMinimal.h"
#include "Tools/UEdMode.h"
#include "SokobanEditorMode.generated.h"

UCLASS()
class USokobanEditorMode : public UEdMode
{
	GENERATED_BODY()

public:
	const static FEditorModeID EM_SokobanEditorMode;

	USokobanEditorMode();

	virtual void Enter() override;
	virtual void Exit() override;
	virtual void CreateToolkit() override;
};
