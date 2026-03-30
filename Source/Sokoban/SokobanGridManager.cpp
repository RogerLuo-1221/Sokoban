// SokobanGridManager.cpp
#include "SokobanGridManager.h"
#include "SokobanPawn.h"
#include "SokobanBoxActor.h"
#include "SokobanTileActor.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Camera/CameraActor.h"

ASokobanGridManager::ASokobanGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

// ============================================================
// Lifecycle
// ============================================================

void ASokobanGridManager::BeginPlay()
{
    Super::BeginPlay();

    // Check for editor play-test temp file first
    FString TempPath = FPaths::ProjectSavedDir() / TEXT("Temp") / TEXT("SokobanPlayTest.json");
    if (FPaths::FileExists(TempPath))
    {
        LoadLevelFromJSON(TempPath);
        IFileManager::Get().Delete(*TempPath);
        bPlayTestMode = true;
        BuildLevel();
    }
    // Otherwise, wait for UI to call LoadCategory() — do NOT auto-load
}

// ============================================================
// Level Loading
// ============================================================

void ASokobanGridManager::SetupCamera()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;
    
    FVector Center = GetGridCenter();
    
    float GridExtent = FMath::Max(Width, Height) * CellSize;
    
    float Padding = 1.4f;
    GridExtent *= Padding;
    
    float HalfFOVRad = FMath::DegreesToRadians(45.f);
    float Distance = (GridExtent * 0.5f) / FMath::Tan(HalfFOVRad);
    
    float PitchDeg = 60.f;
    float PitchRad = FMath::DegreesToRadians(PitchDeg);
    float CamHeight = Distance * FMath::Sin(PitchRad);
    float HorizontalDist = Distance * FMath::Cos(PitchRad);

    FVector CamPos = Center + FVector(-HorizontalDist, 0, CamHeight);
    CamPos.X -= CellSize * 1.5f;
    CamPos.Z += CellSize * 0.5f;

    FRotator CamRot = FRotator(-PitchDeg, 0.f, 0.f);

    PC->SetControlRotation(CamRot);
    
    ACameraActor* Cam = GetWorld()->SpawnActor<ACameraActor>(CamPos, CamRot);
    if (Cam)
    {
        PC->SetViewTargetWithBlend(Cam, 0.f);
    }
}

void ASokobanGridManager::LoadLevelFromJSON(const FString& FilePath)
{
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load level: %s"), *FilePath);
        return;
    }

    TSharedPtr<FJsonObject> JsonObj;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObj) || !JsonObj.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON: %s"), *FilePath);
        return;
    }

    Width = JsonObj->GetIntegerField(TEXT("width"));
    Height = JsonObj->GetIntegerField(TEXT("height"));

    Grid.SetNum(Width * Height);

    // Init all cells
    for (int32 X = 0; X < Height; X++)
    {
        for (int32 Y = 0; Y < Width; Y++)
        {
            FGridCell& Cell = Grid[X * Width + Y];
            Cell.Coordinate = FIntPoint(X, Y);
            Cell.TileType = ETileType::Normal;
            Cell.EntityType = EEntityType::None;
        }
    }

    // Parse tiles (2D array: rows[X][Y])
    const TArray<TSharedPtr<FJsonValue>>& TilesRows = JsonObj->GetArrayField(TEXT("tiles"));
    for (int32 X = 0; X < TilesRows.Num() && X < Height; X++)
    {
        const TArray<TSharedPtr<FJsonValue>>& Row = TilesRows[X]->AsArray();
        for (int32 Y = 0; Y < Row.Num() && Y < Width; Y++)
        {
            Grid[X * Width + Y].TileType = static_cast<ETileType>((int32)Row[Y]->AsNumber());
        }
    }

    // Parse entities (2D array: rows[X][Y])
    const TArray<TSharedPtr<FJsonValue>>& EntitiesRows = JsonObj->GetArrayField(TEXT("entities"));
    for (int32 X = 0; X < EntitiesRows.Num() && X < Height; X++)
    {
        const TArray<TSharedPtr<FJsonValue>>& Row = EntitiesRows[X]->AsArray();
        for (int32 Y = 0; Y < Row.Num() && Y < Width; Y++)
        {
            EEntityType Entity = static_cast<EEntityType>((int32)Row[Y]->AsNumber());
            Grid[X * Width + Y].EntityType = Entity;
            if (Entity == EEntityType::Player)
            {
                PlayerGridPos = FIntPoint(X, Y);
            }
        }
    }

    InitialGrid = Grid;
    InitialPlayerPos = PlayerGridPos;
}

// ============================================================
// Build / Clear Visual Actors
// ============================================================

void ASokobanGridManager::BuildLevel()
{
    ClearLevel();

    UWorld* World = GetWorld();
    if (!World) return;

    for (int32 i = 0; i < Grid.Num(); i++)
    {
        const FGridCell& Cell = Grid[i];
        FVector WorldPos = GridToWorld(Cell.Coordinate);

        // Skip walls for tile actor — wall is just a taller cube
        // Spawn tile actor for every cell (visual ground)
        if (TileClass)
        {
            ASokobanTileActor* Tile = World->SpawnActor<ASokobanTileActor>(
                TileClass, WorldPos, FRotator::ZeroRotator);
            if (Tile)
            {
                Tile->InitTile(Cell.TileType);
                TileActors.Add(Tile);
            }
        }

        // Spawn entities
        if (Cell.EntityType == EEntityType::Player && PawnClass)
        {
            FVector PawnPos = WorldPos + FVector(0, 0, CellSize * 0.5f);
            PlayerPawn = World->SpawnActor<ASokobanPawn>(
                PawnClass, PawnPos, FRotator::ZeroRotator);
        }
        else if (Cell.EntityType == EEntityType::Box && BoxClass)
        {
            FVector BoxPos = WorldPos + FVector(0, 0, CellSize * 0.5f);
            ASokobanBoxActor* Box = World->SpawnActor<ASokobanBoxActor>(
                BoxClass, BoxPos, FRotator::ZeroRotator);
            if (Box)
            {
                BoxActors.Add(Cell.Coordinate, Box);
            }
        }
    }
    
    SetupCamera();
}

void ASokobanGridManager::ClearLevel()
{
    for (ASokobanTileActor* Tile : TileActors)
    {
        if (Tile) Tile->Destroy();
    }
    TileActors.Empty();

    for (auto& Pair : BoxActors)
    {
        if (Pair.Value) Pair.Value->Destroy();
    }
    BoxActors.Empty();

    if (PlayerPawn)
    {
        PlayerPawn->Destroy();
        PlayerPawn = nullptr;
    }
}

// ============================================================
// Movement
// ============================================================

bool ASokobanGridManager::TryMove(EGridDirection Dir)
{
    if (bLevelComplete) return false;

    FIntPoint Offset = DirToOffset(Dir);
    FIntPoint Target = PlayerGridPos + Offset;

    if (!IsValidCoord(Target)) return false;

    const FGridCell& TargetCell = GetCellConst(Target);

    // Wall blocks
    if (TargetCell.TileType == ETileType::Wall) return false;

    bool bPushBox = false;
    FIntPoint BoxFrom = Target;
    FIntPoint BoxFinal = Target;

    // Box push check
    if (TargetCell.EntityType == EEntityType::Box)
    {
        FIntPoint BoxPush = Target + Offset;

        if (!IsValidCoord(BoxPush)) return false;

        const FGridCell& BehindBox = GetCellConst(BoxPush);
        if (BehindBox.TileType == ETileType::Wall) return false;
        if (BehindBox.EntityType == EEntityType::Box) return false;

        bPushBox = true;

        // Compute box final position after ice sliding
        BoxFinal = BoxPush;
        while (GetCellConst(BoxFinal).TileType == ETileType::Ice)
        {
            FIntPoint Next = BoxFinal + Offset;
            if (!IsValidCoord(Next)) break;
            if (GetCellConst(Next).TileType == ETileType::Wall) break;
            if (GetCellConst(Next).EntityType == EEntityType::Box) break;
            BoxFinal = Next;
        }
    }

    // Compute player final position after ice sliding
    // Temporarily update box position in grid so player slide calculation is accurate
    if (bPushBox)
    {
        GetCell(BoxFrom).EntityType = EEntityType::None;
        GetCell(BoxFinal).EntityType = EEntityType::Box;
    }

    FIntPoint PlayerFinal = Target;
    while (GetCellConst(PlayerFinal).TileType == ETileType::Ice)
    {
        FIntPoint Next = PlayerFinal + Offset;
        if (!IsValidCoord(Next)) break;
        if (GetCellConst(Next).TileType == ETileType::Wall) break;
        if (GetCellConst(Next).EntityType != EEntityType::None) break;
        PlayerFinal = Next;
    }

    // Restore grid state (ExecuteMove will do the real update)
    if (bPushBox)
    {
        GetCell(BoxFinal).EntityType = EEntityType::None;
        GetCell(BoxFrom).EntityType = EEntityType::Box;
    }

    ExecuteMove(PlayerFinal, bPushBox, BoxFrom, BoxFinal);
    return true;
}

void ASokobanGridManager::ExecuteMove(
    FIntPoint PlayerTo, bool bPushBox,
    FIntPoint BoxFrom, FIntPoint BoxTo)
{
    FIntPoint PlayerFrom = PlayerGridPos;

    // Record undo before modifying state
    FMoveCommand Cmd;
    Cmd.OldPlayerPos = PlayerFrom;
    Cmd.NewPlayerPos = PlayerTo;
    Cmd.bPushedBox = bPushBox;
    Cmd.OldBoxPos = BoxFrom;
    Cmd.NewBoxPos = BoxTo;
    UndoStack.Push(Cmd);

    // Update box in grid + visual
    if (bPushBox)
    {
        GetCell(BoxFrom).EntityType = EEntityType::None;
        GetCell(BoxTo).EntityType = EEntityType::Box;

        if (TObjectPtr<ASokobanBoxActor>* FoundBox = BoxActors.Find(BoxFrom))
        {
            ASokobanBoxActor* BoxActor = *FoundBox;
            BoxActor->MoveTo(GridToWorld(BoxTo) + FVector(0, 0, CellSize * 0.5f));
            BoxActors.Remove(BoxFrom);
            BoxActors.Add(BoxTo, BoxActor);
        }
    }

    // Update player in grid + visual
    GetCell(PlayerFrom).EntityType = EEntityType::None;
    GetCell(PlayerTo).EntityType = EEntityType::Player;
    PlayerGridPos = PlayerTo;

    if (PlayerPawn)
    {
        PlayerPawn->MoveTo(GridToWorld(PlayerTo) + FVector(0, 0, CellSize * 0.5f));
    }

    CheckWinCondition();
}

// ============================================================
// Undo & Reset
// ============================================================

void ASokobanGridManager::Undo()
{
    if (UndoStack.Num() == 0) return;

    FMoveCommand Cmd = UndoStack.Pop();

    // Reverse player
    GetCell(Cmd.NewPlayerPos).EntityType = EEntityType::None;
    GetCell(Cmd.OldPlayerPos).EntityType = EEntityType::Player;
    PlayerGridPos = Cmd.OldPlayerPos;

    if (PlayerPawn)
    {
        PlayerPawn->MoveTo(GridToWorld(Cmd.OldPlayerPos) + FVector(0, 0, CellSize * 0.5f));
    }

    // Reverse box
    if (Cmd.bPushedBox)
    {
        GetCell(Cmd.NewBoxPos).EntityType = EEntityType::None;
        GetCell(Cmd.OldBoxPos).EntityType = EEntityType::Box;

        if (TObjectPtr<ASokobanBoxActor>* FoundBox = BoxActors.Find(Cmd.NewBoxPos))
        {
            ASokobanBoxActor* BoxActor = *FoundBox;
            BoxActor->MoveTo(GridToWorld(Cmd.OldBoxPos) + FVector(0, 0, CellSize * 0.5f));
            BoxActors.Remove(Cmd.NewBoxPos);
            BoxActors.Add(Cmd.OldBoxPos, BoxActor);
        }
    }
}

void ASokobanGridManager::ResetLevel()
{
    // Restore grid to initial snapshot
    Grid = InitialGrid;
    PlayerGridPos = InitialPlayerPos;
    UndoStack.Empty();

    // Rebuild all visuals
    BuildLevel();
}

// ============================================================
// Win Condition
// ============================================================

void ASokobanGridManager::CheckWinCondition()
{
    int32 TotalPads = 0;
    int32 CoveredPads = 0;

    for (const FGridCell& Cell : Grid)
    {
        if (Cell.TileType == ETileType::TargetPad)
        {
            TotalPads++;
            if (Cell.EntityType == EEntityType::Box)
            {
                CoveredPads++;
            }
        }
    }

    if (TotalPads > 0 && CoveredPads == TotalPads)
    {
        bLevelComplete = true;
        OnLevelComplete.Broadcast();
    }
}

// ============================================================
// IsMoving — check if any actor is mid-lerp
// ============================================================

bool ASokobanGridManager::IsMoving() const
{
    if (PlayerPawn && PlayerPawn->IsLerping()) return true;

    for (const auto& Pair : BoxActors)
    {
        if (Pair.Value && Pair.Value->IsLerping()) return true;
    }

    return false;
}

// ============================================================
// Coordinate Helpers
// ============================================================

FGridCell& ASokobanGridManager::GetCell(FIntPoint Coord)
{
    return Grid[Coord.X * Width + Coord.Y];
}

const FGridCell& ASokobanGridManager::GetCellConst(FIntPoint Coord) const
{
    return Grid[Coord.X * Width + Coord.Y];
}

bool ASokobanGridManager::IsValidCoord(FIntPoint Coord) const
{
    return Coord.X >= 0 && Coord.X < Height &&
           Coord.Y >= 0 && Coord.Y < Width;
}

FIntPoint ASokobanGridManager::DirToOffset(EGridDirection Dir) const
{
    switch (Dir)
    {
        case EGridDirection::North: return FIntPoint(1, 0);
        case EGridDirection::South: return FIntPoint(-1, 0);
        case EGridDirection::East:  return FIntPoint(0, 1);
        case EGridDirection::West:  return FIntPoint(0, -1);
        default: return FIntPoint::ZeroValue;
    }
}

FVector ASokobanGridManager::GridToWorld(FIntPoint GridPos) const
{
    return FVector(GridPos.X * CellSize, GridPos.Y * CellSize, 0.f);
}

FIntPoint ASokobanGridManager::WorldToGrid(FVector WorldPos) const
{
    return FIntPoint(
        FMath::FloorToInt(WorldPos.X / CellSize),
        FMath::FloorToInt(WorldPos.Y / CellSize));
}

FVector ASokobanGridManager::GetGridCenter() const
{
    return FVector(
        (Height - 1) * CellSize * 0.5f,
        (Width - 1) * CellSize * 0.5f,
        0.f);
}

// ============================================================
// Level Flow
// ============================================================

void ASokobanGridManager::LoadLevelByIndex(int32 Index)
{
    if (!LevelConfig || !LevelConfig->Levels.IsValidIndex(Index)) return;

    CurrentLevelIndex = Index;
    bLevelComplete = false;
    UndoStack.Empty();

    const FLevelEntry& Entry = LevelConfig->Levels[Index];
    FString FullPath = FPaths::ProjectContentDir() / Entry.JsonFile.FilePath;

    LoadLevelFromJSON(FullPath);
}

void ASokobanGridManager::LoadCategory(ELevelCategory Category)
{
    if (!LevelConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("GridManager: LevelConfig DataAsset is not set!"));
        return;
    }

    int32 Index = static_cast<int32>(Category);
    if (!LevelConfig->Levels.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Warning, TEXT("GridManager: No level at index %d"), Index);
        return;
    }

    ClearLevel();
    LoadLevelByIndex(Index);
    BuildLevel();
}

void ASokobanGridManager::LoadNextLevel()
{
    if (bPlayTestMode)
    {
        return;
    }

    int32 NextIndex = CurrentLevelIndex + 1;
    if (!LevelConfig || NextIndex >= LevelConfig->Levels.Num())
    {
        return;
    }

    ClearLevel();
    LoadLevelByIndex(NextIndex);
    BuildLevel();
}

bool ASokobanGridManager::IsLastLevel() const
{
    return !LevelConfig || CurrentLevelIndex >= LevelConfig->Levels.Num() - 1;
}

int32 ASokobanGridManager::GetTotalLevelCount() const
{
    return LevelConfig ? LevelConfig->Levels.Num() : 0;
}