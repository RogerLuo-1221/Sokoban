// SokobanPawn.cpp
#include "SokobanPawn.h"

ASokobanPawn::ASokobanPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CubeMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(CubeMesh.Object);
	}

	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorScale3D(FVector(0.8f, 0.8f, 0.8f));

	// Player color: blue
	// Will be set by dynamic material in BeginPlay or GridManager
}

void ASokobanPawn::MoveTo(FVector TargetWorldPos)
{
	LerpTarget = TargetWorldPos;
	bIsLerping = true;
}

void ASokobanPawn::Tick(float DeltaTime)
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