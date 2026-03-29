// SokobanTypes.h
#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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

UENUM(BlueprintType)
enum class ELevelCategory : uint8
{
	Tutorial,
	Beginner,
	Intermediate,
	Advanced
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

// ============================================================
// Level Config DataAsset
// ============================================================

/** A single level entry: display name + path to JSON file. */
USTRUCT(BlueprintType)
struct FLevelEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DisplayName;

	/**
	 * JSON level file, relative to Content directory.
	 * Click the "..." button to browse. Only .json files are shown.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (RelativeToGameContentDir, FilePathFilter = "json"))
	FFilePath JsonFile;
};

/**
 * Central config asset. A flat array of levels.
 * Index 0 = Tutorial, 1 = Beginner, 2 = Intermediate, 3 = Advanced.
 * ELevelCategory enum value maps directly to the array index.
 */
UCLASS(BlueprintType)
class SOKOBAN_API USokobanLevelConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** All levels in order: [0]=Tutorial [1]=Beginner [2]=Intermediate [3]=Advanced */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Levels")
	TArray<FLevelEntry> Levels;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);

		const FString ContentDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
		for (FLevelEntry& Entry : Levels)
		{
			FString& Path = Entry.JsonFile.FilePath;
			FString FullPath = FPaths::ConvertRelativePathToFull(Path);
			if (FullPath.StartsWith(ContentDir))
			{
				Path = FullPath.RightChop(ContentDir.Len());
			}
		}
	}
#endif
};