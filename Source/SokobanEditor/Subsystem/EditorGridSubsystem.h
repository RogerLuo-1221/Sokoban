#pragma once

#include "CoreMinimal.h"
#include "EditorSubsystem.h"
#include "Sokoban/SokobanTypes.h"
#include "EditorGridSubsystem.generated.h"

class ASokobanTileActor;
class ASokobanPawn;
class ASokobanBoxActor;

struct FGridSnapshot
{
	int32 Width;
	int32 Height;
	TArray<FGridCell> Grid;
};

UENUM(BlueprintType)
enum class EPaintMode : uint8
{
	TileType,
	EntityType,
	Erase
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGridChanged);

UCLASS()
class UEditorGridSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	// --- Edit Mode Lifecycle ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void EnterEditMode();

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void ExitEditMode();

	// --- Grid Management ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void CreateNewGrid(int32 InWidth, int32 InHeight);

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void ResizeGrid(int32 NewWidth, int32 NewHeight);

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void ClearGrid();

	// --- Paint API (called by Tools) ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void PaintAt(FIntPoint Coord);

	// --- Active Selection (set by EUW panel) ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SetPaintMode(EPaintMode Mode) { CurrentPaintMode = Mode; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SetActiveTileType(ETileType Type) { ActiveTileType = Type; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SetActiveEntityType(EEntityType Type) { ActiveEntityType = Type; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	EPaintMode GetPaintMode() const { return CurrentPaintMode; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	ETileType GetActiveTileType() const { return ActiveTileType; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	EEntityType GetActiveEntityType() const { return ActiveEntityType; }

	// --- Coordinates & Dimensions ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	bool IsValidCoord(FIntPoint Coord) const;

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	float GetCellSize() const { return CellSize; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	int32 GetGridWidth() const { return Width; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	int32 GetGridHeight() const { return Height; }

	// --- JSON I/O ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SaveToJSON(const FString& FullPath);

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void LoadFromJSON(const FString& FullPath);

	/** Opens a Save File dialog, then saves. */
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SaveWithDialog();

	/** Opens an Open File dialog, then loads the selected JSON. */
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void LoadWithDialog();

	// --- Undo ---
	void Undo();
	void BeginStroke();
	void EndStroke();

	// --- Play Test ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void PlayTest();

	// --- Grid Data Query (for EUW) ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	FGridCell GetCellData(FIntPoint Coord) const;

	// --- Grid Changed Delegate (EUW binds to this) ---
	UPROPERTY(BlueprintAssignable, Category = "SokobanEditor")
	FOnGridChanged OnGridChanged;

	// --- Level Name ---
	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	void SetLevelName(const FString& Name) { LevelName = Name; }

	UFUNCTION(BlueprintCallable, Category = "SokobanEditor")
	FString GetLevelName() const { return LevelName; }

private:
	UPROPERTY()
	TArray<FGridCell> Grid;

	int32 Width = 8;
	int32 Height = 8;
	float CellSize = 100.f;
	FString LevelName = TEXT("Untitled");

	EPaintMode CurrentPaintMode = EPaintMode::TileType;
	ETileType ActiveTileType = ETileType::Wall;
	EEntityType ActiveEntityType = EEntityType::Player;

	// --- Preview Actors ---
	UPROPERTY()
	TArray<TObjectPtr<ASokobanTileActor>> PreviewActors;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> EntityPreviewActors;

	// --- BP Class References ---
	UPROPERTY()
	TSubclassOf<ASokobanTileActor> TileClass;

	UPROPERTY()
	TSubclassOf<ASokobanPawn> PawnClass;

	UPROPERTY()
	TSubclassOf<ASokobanBoxActor> BoxClass;

	// --- PIE Delegate Handle ---
	FDelegateHandle EndPIEHandle;
	void OnEndPIE(bool bIsSimulating);

	// --- Validation ---
	bool ValidateLevel(FString& OutError) const;

	// --- Undo Stack ---
	static constexpr int32 MaxUndoSteps = 50;
	TArray<FGridSnapshot> UndoStack;
	bool bStrokeActive = false;
	void PushSnapshot();

	// --- Internal Helpers ---
	void LoadBlueprintClasses();
	void RebuildPreview();
	void ClearPreview();
	FGridCell& GetCell(FIntPoint Coord);
	FVector GridToWorld(FIntPoint Coord) const;
};
