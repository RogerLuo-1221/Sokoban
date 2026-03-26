// SokobanPawn.h
#pragma once
#include "CoreMinimal.h"
#include "SokobanMovableActor.h"
#include "SokobanPawn.generated.h"

UCLASS()
class SOKOBAN_API ASokobanPawn : public APawn
{
	GENERATED_BODY()

public:
	ASokobanPawn();

	void MoveTo(FVector TargetWorldPos);
	bool IsLerping() const { return bIsLerping; }

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 800.f;

	FVector LerpTarget;
	bool bIsLerping = false;
};