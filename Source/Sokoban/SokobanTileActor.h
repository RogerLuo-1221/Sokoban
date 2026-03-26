// SokobanTileActor.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SokobanTypes.h"
#include "SokobanTileActor.generated.h"

UCLASS()
class SOKOBAN_API ASokobanTileActor : public AActor
{
	GENERATED_BODY()

public:
	ASokobanTileActor();
	void InitTile(ETileType Type);
	
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInstance> MI_Floor;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInstance> MI_Wall;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInstance> MI_Ice;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	TObjectPtr<UMaterialInstance> MI_TargetPad;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;
};