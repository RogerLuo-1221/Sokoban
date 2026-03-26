// SokobanTypes.h
#pragma once
#include "CoreMinimal.h"
#include "SokobanTypes.generated.h"

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Normal,
	Wall,
	Ice,
	TargetPad
};

UENUM(BlueprintType)
enum class EEntityType : uint8
{
	None,
	Box,
	Player
};

UENUM(BlueprintType)
enum class EGridDirection : uint8
{
	North,
	East,
	South,
	West
};

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Coordinate = FIntPoint::ZeroValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETileType TileType = ETileType::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEntityType EntityType = EEntityType::None;
};

USTRUCT(BlueprintType)
struct FMoveCommand
{
	GENERATED_BODY()

	FIntPoint OldPlayerPos;
	FIntPoint NewPlayerPos;
	bool bPushedBox = false;
	FIntPoint OldBoxPos;
	FIntPoint NewBoxPos;
};

USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY()
	FString LevelName;

	UPROPERTY()
	int32 Width = 0;

	UPROPERTY()
	int32 Height = 0;

	UPROPERTY()
	TArray<FGridCell> Cells;
};