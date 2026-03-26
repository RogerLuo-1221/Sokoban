// SokobanGridManager.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SokobanTypes.h"
#include "SokobanGridManager.generated.h"

class ASokobanPawn;
class ASokobanBoxActor;
class ASokobanTileActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelComplete);

UCLASS()
class SOKOBAN_API ASokobanGridManager : public AActor
{
    GENERATED_BODY()

public:
    ASokobanGridManager();

    // --- Public API called by PlayerController ---
    bool TryMove(EGridDirection Dir);
    void Undo();
    void ResetLevel();
    bool IsMoving() const;

    // --- Level loading ---
    void LoadLevelFromJSON(const FString& FilePath);
    void BuildLevel();
    void ClearLevel();

    // --- Helpers ---
    FVector GridToWorld(FIntPoint GridPos) const;
    FIntPoint WorldToGrid(FVector WorldPos) const;
    FVector GetGridCenter() const;
    float GetCellSize() const { return CellSize; }
    int32 GetGridWidth() const { return Width; }
    int32 GetGridHeight() const { return Height; }

    UPROPERTY(BlueprintAssignable)
    FOnLevelComplete OnLevelComplete;

protected:
    virtual void BeginPlay() override;

private:
    // --- Grid data ---
    UPROPERTY()
    TArray<FGridCell> Grid;

    int32 Width = 0;
    int32 Height = 0;
    FIntPoint PlayerGridPos;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float CellSize = 100.f;

    // --- Visual actors ---
    UPROPERTY()
    TObjectPtr<ASokobanPawn> PlayerPawn;

    UPROPERTY()
    TMap<FIntPoint, TObjectPtr<ASokobanBoxActor>> BoxActors;

    UPROPERTY()
    TArray<TObjectPtr<ASokobanTileActor>> TileActors;

    // --- Subclass references (set in BP or constructor) ---
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<ASokobanPawn> PawnClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<ASokobanBoxActor> BoxClass;

    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<ASokobanTileActor> TileClass;

    // --- Undo ---
    TArray<FMoveCommand> UndoStack;

    // --- Snapshot for reset ---
    TArray<FGridCell> InitialGrid;
    FIntPoint InitialPlayerPos;

    // --- Internal helpers ---
    FGridCell& GetCell(FIntPoint Coord);
    const FGridCell& GetCellConst(FIntPoint Coord) const;
    bool IsValidCoord(FIntPoint Coord) const;
    FIntPoint DirToOffset(EGridDirection Dir) const;

    void ExecuteMove(FIntPoint PlayerTo, bool bPushBox,
                     FIntPoint BoxFrom, FIntPoint BoxTo);
    void CheckWinCondition();

    // --- Default level for testing ---
    void LoadDefaultLevel();
    
    // --- Camera Setting ---
    void SetupCamera();
};