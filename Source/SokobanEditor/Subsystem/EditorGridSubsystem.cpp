#include "EditorGridSubsystem.h"
#include "Sokoban/SokobanTileActor.h"
#include "Sokoban/SokobanPawn.h"
#include "Sokoban/SokobanBoxActor.h"
#include "Editor.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/MessageDialog.h"
#include "EditorActorFolders.h"

void UEditorGridSubsystem::EnterEditMode()
{
	UndoStack.Empty();
	LoadBlueprintClasses();
	CreateNewGrid(Width, Height);
	RebuildPreview();
}

void UEditorGridSubsystem::ExitEditMode()
{
	ClearPreview();
	Grid.Empty();
}

void UEditorGridSubsystem::CreateNewGrid(int32 InWidth, int32 InHeight)
{
	Width = InWidth;
	Height = InHeight;
	Grid.SetNum(Width * Height);

	for (int32 X = 0; X < Height; ++X)
	{
		for (int32 Y = 0; Y < Width; ++Y)
		{
			FGridCell& Cell = Grid[X * Width + Y];
			Cell.Coordinate = FIntPoint(X, Y);
			Cell.TileType = ETileType::Normal;
			Cell.EntityType = EEntityType::None;
		}
	}

	OnGridChanged.Broadcast();
}

// --- Undo ---

void UEditorGridSubsystem::PushSnapshot()
{
	FGridSnapshot Snap;
	Snap.Width = Width;
	Snap.Height = Height;
	Snap.Grid = Grid;
	UndoStack.Add(MoveTemp(Snap));

	if (UndoStack.Num() > MaxUndoSteps)
	{
		UndoStack.RemoveAt(0);
	}
}

void UEditorGridSubsystem::Undo()
{
	if (UndoStack.IsEmpty()) return;

	FGridSnapshot Snap = UndoStack.Pop();
	Width = Snap.Width;
	Height = Snap.Height;
	Grid = MoveTemp(Snap.Grid);

	OnGridChanged.Broadcast();
	RebuildPreview();
}

void UEditorGridSubsystem::BeginStroke()
{
	PushSnapshot();
	bStrokeActive = true;
}

void UEditorGridSubsystem::EndStroke()
{
	bStrokeActive = false;
}

// --- Grid Management ---

void UEditorGridSubsystem::ResizeGrid(int32 NewWidth, int32 NewHeight)
{
	PushSnapshot();
	TArray<FGridCell> OldGrid = Grid;
	int32 OldWidth = Width;
	int32 OldHeight = Height;

	CreateNewGrid(NewWidth, NewHeight);

	// Copy old data that fits
	int32 CopyW = FMath::Min(OldWidth, NewWidth);     // Y range (columns)
	int32 CopyH = FMath::Min(OldHeight, NewHeight);   // X range (rows)
	for (int32 X = 0; X < CopyH; ++X)
	{
		for (int32 Y = 0; Y < CopyW; ++Y)
		{
			Grid[X * Width + Y] = OldGrid[X * OldWidth + Y];
		}
	}

	RebuildPreview();
}

void UEditorGridSubsystem::ClearGrid()
{
	PushSnapshot();
	CreateNewGrid(Width, Height);
	RebuildPreview();
}

void UEditorGridSubsystem::PaintAt(FIntPoint Coord)
{
	if (!IsValidCoord(Coord)) return;

	FGridCell& Cell = GetCell(Coord);

	switch (CurrentPaintMode)
	{
	case EPaintMode::TileType:
		Cell.TileType = ActiveTileType;
		break;
	case EPaintMode::EntityType:
		// Player must be unique — remove any existing player before placing new one
		if (ActiveEntityType == EEntityType::Player)
		{
			for (FGridCell& C : Grid)
			{
				if (C.EntityType == EEntityType::Player)
					C.EntityType = EEntityType::None;
			}
		}
		Cell.EntityType = ActiveEntityType;
		break;
	case EPaintMode::Erase:
		Cell.TileType = ETileType::Normal;
		Cell.EntityType = EEntityType::None;
		break;
	}

	OnGridChanged.Broadcast();
	RebuildPreview();
}

FGridCell UEditorGridSubsystem::GetCellData(FIntPoint Coord) const
{
	if (IsValidCoord(Coord))
	{
		return Grid[Coord.X * Width + Coord.Y];
	}
	return FGridCell();
}

bool UEditorGridSubsystem::IsValidCoord(FIntPoint Coord) const
{
	return Coord.X >= 0 && Coord.X < Height && Coord.Y >= 0 && Coord.Y < Width;
}

void UEditorGridSubsystem::SaveToJSON(const FString& FullPath)
{
	TSharedRef<FJsonObject> JsonObj = MakeShared<FJsonObject>();
	JsonObj->SetStringField("name", LevelName);
	JsonObj->SetNumberField("width", Width);
	JsonObj->SetNumberField("height", Height);
	JsonObj->SetStringField("_地格索引", "0=空地 1=墙体 2=冰面 3=目标点");
	JsonObj->SetStringField("_实体索引", "0=无 1=玩家 2=箱子");

	// 2D arrays: outer = rows (GridX, 0..Height-1), inner = columns (GridY, 0..Width-1)
	TArray<TSharedPtr<FJsonValue>> TilesRows;
	TArray<TSharedPtr<FJsonValue>> EntitiesRows;
	for (int32 X = 0; X < Height; X++)
	{
		TArray<TSharedPtr<FJsonValue>> TileRow, EntityRow;
		for (int32 Y = 0; Y < Width; Y++)
		{
			const FGridCell& Cell = Grid[X * Width + Y];
			TileRow.Add(MakeShared<FJsonValueNumber>((int32)Cell.TileType));
			EntityRow.Add(MakeShared<FJsonValueNumber>((int32)Cell.EntityType));
		}
		TilesRows.Add(MakeShared<FJsonValueArray>(TileRow));
		EntitiesRows.Add(MakeShared<FJsonValueArray>(EntityRow));
	}
	JsonObj->SetArrayField("tiles", TilesRows);
	JsonObj->SetArrayField("entities", EntitiesRows);

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObj, Writer);

	FFileHelper::SaveStringToFile(OutputString, *FullPath);
}

void UEditorGridSubsystem::LoadFromJSON(const FString& FullPath)
{
	PushSnapshot();
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("EditorGridSubsystem: LoadFromJSON failed to read %s"), *FullPath);
		return;
	}

	TSharedPtr<FJsonObject> JsonObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObj) || !JsonObj.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("EditorGridSubsystem: LoadFromJSON failed to parse JSON"));
		return;
	}

	Width = JsonObj->GetIntegerField("width");
	Height = JsonObj->GetIntegerField("height");
	Grid.SetNum(Width * Height);

	// Init cell coordinates
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
	const TArray<TSharedPtr<FJsonValue>>& TilesRows = JsonObj->GetArrayField("tiles");
	for (int32 X = 0; X < TilesRows.Num() && X < Height; X++)
	{
		const TArray<TSharedPtr<FJsonValue>>& Row = TilesRows[X]->AsArray();
		for (int32 Y = 0; Y < Row.Num() && Y < Width; Y++)
		{
			Grid[X * Width + Y].TileType = static_cast<ETileType>((int32)Row[Y]->AsNumber());
		}
	}

	// Parse entities (2D array: rows[X][Y])
	const TArray<TSharedPtr<FJsonValue>>& EntitiesRows = JsonObj->GetArrayField("entities");
	for (int32 X = 0; X < EntitiesRows.Num() && X < Height; X++)
	{
		const TArray<TSharedPtr<FJsonValue>>& Row = EntitiesRows[X]->AsArray();
		for (int32 Y = 0; Y < Row.Num() && Y < Width; Y++)
		{
			Grid[X * Width + Y].EntityType = static_cast<EEntityType>((int32)Row[Y]->AsNumber());
		}
	}

	// Extract level name from file path
	LevelName = FPaths::GetBaseFilename(FullPath);
	OnGridChanged.Broadcast();
	RebuildPreview();
}

static FString GetDefaultLevelDataDir()
{
	return FPaths::ProjectContentDir() / TEXT("Levels") / TEXT("LevelData");
}

void UEditorGridSubsystem::SaveWithDialog()
{
	// Hard constraint: block save if level is invalid
	FString Error;
	if (!ValidateLevel(Error))
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(NSLOCTEXT("SokobanEditor", "SaveValidation", "无法保存：{0}"),
			FText::FromString(Error)));
		return;
	}

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform) return;

	const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	FString DefaultDir = GetDefaultLevelDataDir();
	IFileManager::Get().MakeDirectory(*DefaultDir, true);

	TArray<FString> OutFiles;
	bool bOpened = DesktopPlatform->SaveFileDialog(
		ParentWindowHandle,
		TEXT("保存推箱子关卡"),
		DefaultDir,
		LevelName + TEXT(".json"),
		TEXT("JSON 文件 (*.json)|*.json"),
		0,
		OutFiles);

	if (bOpened && OutFiles.Num() > 0)
	{
		LevelName = FPaths::GetBaseFilename(OutFiles[0]);
		SaveToJSON(OutFiles[0]);
	}
}

void UEditorGridSubsystem::LoadWithDialog()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform) return;

	const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	FString DefaultDir = GetDefaultLevelDataDir();

	TArray<FString> OutFiles;
	bool bOpened = DesktopPlatform->OpenFileDialog(
		ParentWindowHandle,
		TEXT("加载推箱子关卡"),
		DefaultDir,
		TEXT(""),
		TEXT("JSON 文件 (*.json)|*.json"),
		0,
		OutFiles);

	if (bOpened && OutFiles.Num() > 0)
	{
		LoadFromJSON(OutFiles[0]);
	}
}

bool UEditorGridSubsystem::ValidateLevel(FString& OutError) const
{
	int32 Players = 0, Boxes = 0, Targets = 0;
	for (const FGridCell& Cell : Grid)
	{
		if (Cell.EntityType == EEntityType::Player) Players++;
		if (Cell.EntityType == EEntityType::Box) Boxes++;
		if (Cell.TileType == ETileType::TargetPad) Targets++;
	}
	if (Players == 0) { OutError = TEXT("未放置玩家！"); return false; }
	if (Boxes == 0) { OutError = TEXT("未放置箱子！"); return false; }
	if (Targets == 0) { OutError = TEXT("未放置目标点！"); return false; }
	if (Boxes != Targets)
	{
		OutError = FString::Printf(TEXT("箱子数量（%d）与目标点数量（%d）不一致！"), Boxes, Targets);
		return false;
	}
	return true;
}

void UEditorGridSubsystem::PlayTest()
{
	// Validate level before play test
	FString Error;
	if (!ValidateLevel(Error))
	{
		FMessageDialog::Open(EAppMsgType::Ok,
			FText::Format(NSLOCTEXT("SokobanEditor", "PlayTestValidation", "无法试玩：{0}"),
			FText::FromString(Error)));
		return;
	}

	// 1. Save current grid to a temp file the game can pick up
	FString TempDir = FPaths::ProjectSavedDir() / TEXT("Temp");
	IFileManager::Get().MakeDirectory(*TempDir, true);
	FString TempPath = TempDir / TEXT("SokobanPlayTest.json");
	SaveToJSON(TempPath);

	// 2. Hide editor preview so it doesn't overlap with game actors
	ClearPreview();

	// 3. Bind callback to restore preview when PIE ends
	if (!EndPIEHandle.IsValid())
	{
		EndPIEHandle = FEditorDelegates::EndPIE.AddUObject(this, &UEditorGridSubsystem::OnEndPIE);
	}

	// 4. Launch PIE
	if (GEditor)
	{
		FRequestPlaySessionParams Params;
		Params.WorldType = EPlaySessionWorldType::PlayInEditor;
		GEditor->RequestPlaySession(Params);
	}
}

void UEditorGridSubsystem::OnEndPIE(bool bIsSimulating)
{
	RebuildPreview();

	// Unbind so we don't accumulate delegates
	FEditorDelegates::EndPIE.Remove(EndPIEHandle);
	EndPIEHandle.Reset();
}

void UEditorGridSubsystem::LoadBlueprintClasses()
{
	if (!TileClass)
	{
		TileClass = LoadClass<ASokobanTileActor>(nullptr,
			TEXT("/Game/Blueprints/BP_SokobanTile.BP_SokobanTile_C"));
	}
	if (!PawnClass)
	{
		PawnClass = LoadClass<ASokobanPawn>(nullptr,
			TEXT("/Game/Blueprints/BP_SokobanPawn.BP_SokobanPawn_C"));
	}
	if (!BoxClass)
	{
		BoxClass = LoadClass<ASokobanBoxActor>(nullptr,
			TEXT("/Game/Blueprints/BP_SokobanBox.BP_SokobanBox_C"));
	}

}

void UEditorGridSubsystem::RebuildPreview()
{
	ClearPreview();

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World) return;

	const FName FolderPath(TEXT("Sokoban Level Preview"));

	for (const FGridCell& Cell : Grid)
	{
		FVector WorldPos = GridToWorld(Cell.Coordinate);

		// Spawn tile for every cell
		if (TileClass)
		{
			ASokobanTileActor* Tile = World->SpawnActor<ASokobanTileActor>(
				TileClass, WorldPos, FRotator::ZeroRotator);
			if (Tile)
			{
				Tile->SetFlags(RF_Transient);
				Tile->InitTile(Cell.TileType);
				Tile->SetFolderPath(FolderPath);
				PreviewActors.Add(Tile);
			}
		}

		// Spawn entity on top
		FVector EntityPos = WorldPos + FVector(0, 0, CellSize * 0.5f);
		if (Cell.EntityType == EEntityType::Player && PawnClass)
		{
			AActor* Pawn = World->SpawnActor<AActor>(PawnClass, EntityPos, FRotator::ZeroRotator);
			if (Pawn) { Pawn->SetFlags(RF_Transient); Pawn->SetFolderPath(FolderPath); EntityPreviewActors.Add(Pawn); }
		}
		else if (Cell.EntityType == EEntityType::Box && BoxClass)
		{
			AActor* Box = World->SpawnActor<AActor>(BoxClass, EntityPos, FRotator::ZeroRotator);
			if (Box) { Box->SetFlags(RF_Transient); Box->SetFolderPath(FolderPath); EntityPreviewActors.Add(Box); }
		}
	}
}

void UEditorGridSubsystem::ClearPreview()
{
	for (ASokobanTileActor* Actor : PreviewActors)
	{
		if (Actor) Actor->Destroy();
	}
	PreviewActors.Empty();

	for (AActor* Actor : EntityPreviewActors)
	{
		if (Actor) Actor->Destroy();
	}
	EntityPreviewActors.Empty();

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (World)
	{
		FActorFolders::Get().DeleteFolder(*World, FName(TEXT("Sokoban Level Preview")));
	}
}

FGridCell& UEditorGridSubsystem::GetCell(FIntPoint Coord)
{
	return Grid[Coord.X * Width + Coord.Y];
}

FVector UEditorGridSubsystem::GridToWorld(FIntPoint Coord) const
{
	// GridX=up(Height)→WorldX, GridY=right(Width)→WorldY
	return FVector(Coord.X * CellSize, Coord.Y * CellSize, 0.f);
}



