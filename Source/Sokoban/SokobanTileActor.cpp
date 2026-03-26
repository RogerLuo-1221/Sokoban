// SokobanTileActor.cpp
#include "SokobanTileActor.h"

ASokobanTileActor::ASokobanTileActor()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

	// Default cube mesh — will be a flat tile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}

	// Flatten: 1x1 tile, 0.1 tall (scale applied to 100-unit cube)
	SetActorScale3D(FVector(1.f, 1.f, 0.1f));

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASokobanTileActor::InitTile(ETileType Type)
{
	UMaterialInstance* Mat = nullptr;
	switch (Type)
	{
	case ETileType::Normal:    Mat = MI_Floor; break;
	case ETileType::Wall:      Mat = MI_Wall;
		SetActorScale3D(FVector(1.f, 1.f, 1.f));
		break;
	case ETileType::Ice:       Mat = MI_Ice; break;
	case ETileType::TargetPad: Mat = MI_TargetPad; break;
	}

	if (Mat)
	{
		MeshComp->SetMaterial(0, Mat);
	}
}