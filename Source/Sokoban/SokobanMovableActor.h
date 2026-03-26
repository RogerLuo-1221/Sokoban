// SokobanMovableActor.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SokobanMovableActor.generated.h"

UCLASS(Abstract)
class SOKOBAN_API ASokobanMovableActor : public AActor
{
	GENERATED_BODY()

public:
	ASokobanMovableActor();

	void MoveTo(FVector TargetWorldPos);
	bool IsLerping() const { return bIsLerping; }

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 800.f;

private:
	FVector LerpTarget;
	bool bIsLerping = false;
};