// SokobanMovableActor.cpp
#include "SokobanMovableActor.h"

ASokobanMovableActor::ASokobanMovableActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Slightly smaller than a full tile so there's visual gap
	SetActorScale3D(FVector(0.8f, 0.8f, 0.8f));
}

void ASokobanMovableActor::MoveTo(FVector TargetWorldPos)
{
	LerpTarget = TargetWorldPos;
	bIsLerping = true;
}

void ASokobanMovableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsLerping) return;

	FVector Current = GetActorLocation();
	FVector New = FMath::VInterpConstantTo(Current, LerpTarget, DeltaTime, MoveSpeed);
	SetActorLocation(New);

	if (FVector::Dist(New, LerpTarget) < 0.5f)
	{
		SetActorLocation(LerpTarget);
		bIsLerping = false;
	}
}