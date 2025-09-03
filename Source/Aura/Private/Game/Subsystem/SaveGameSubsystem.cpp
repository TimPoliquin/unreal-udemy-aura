// Copyright Alien Shores


#include "Game/Subsystem/SaveGameSubsystem.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameState.h"
#include "Game/Save/AuraSaveGame.h"
#include "Game/Subsystem/LevelGameInstanceSubsystem.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogAura, Warning, TEXT("[%s] Initialize"), *GetName())
	if (bAutoSaveOnStart)
	{
		AutoSaveTransient();
	}
}

void USaveGameSubsystem::Deinitialize()
{
	if (CurrentSaveState.SaveState == EAuraGameSaveState::Undefined || CurrentSaveState.SaveState ==
		EAuraGameSaveState::Transient)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Deleting save game: %s"), *GetName(), *CurrentSaveState.SlotName);
		UGameplayStatics::DeleteGameInSlot(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
	}
	Super::Deinitialize();
}

USaveGameSubsystem* USaveGameSubsystem::Get(
	const UObject* WorldContextObject
)
{
	if (IsValid(WorldContextObject))
	{
		return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<USaveGameSubsystem>();
	}
	UE_LOG(LogAura, Error, TEXT("[%s] Invalid local player"), *FString("LocalPlayerSaveGameSubsystem::Get"));
	return nullptr;
}

UAuraSaveGame* USaveGameSubsystem::GetInGameSaveData() const
{
	return GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UAuraSaveGame* USaveGameSubsystem::CreateDefaultSaveData(
	const int32 SlotIndex,
	const FString& SlotName
) const
{
	const UObject* WorldContextObject = GetGameInstance()->GetWorld();
	const ULevelGameInstanceSubsystem* LevelGameInstanceSubsystem = ULevelGameInstanceSubsystem::Get(WorldContextObject);
	UAuraSaveGame* SaveGame = Cast<UAuraSaveGame>(
		UGameplayStatics::CreateSaveGameObject(SaveGameClass)
	);
	SaveGame->SlotIndex = SlotIndex;
	SaveGame->SlotName = SlotName;
	SaveGame->PlayerName = SlotName;
	SaveGame->MapAssetName = LevelGameInstanceSubsystem->GetDefaultMapAssetName();
	SaveGame->MapName = LevelGameInstanceSubsystem->GetDefaultMapName();
	SaveGame->SaveSlotStatus = Taken;
	SaveGame->PlayerStartTag = LevelGameInstanceSubsystem->GetDefaultPlayerStartTag(SaveGame->MapName);
	SaveGame->PlayerLevel = LevelGameInstanceSubsystem->GetDefaultPlayerLevel(SaveGame->MapName);
	return SaveGame;
}

void USaveGameSubsystem::InitializeSaveState(const UAuraSaveGame* SaveGame, const bool bIsTransient)
{
	InitializeSaveState(SaveGame->PlayerStartTag, SaveGame->SlotName, SaveGame->SlotIndex, bIsTransient);
}

void USaveGameSubsystem::InitializeSaveState(
	const FName& InPlayerStartTag,
	const FString& InSlotName,
	const int32 InSlotIndex,
	bool bIsTransient
)
{
	CurrentSaveState.SlotName = InSlotName;
	CurrentSaveState.SlotIndex = InSlotIndex;
	CurrentSaveState.PlayerStartTag = InPlayerStartTag;
	CurrentSaveState.SaveState = bIsTransient
		                             ? EAuraGameSaveState::Transient
		                             : EAuraGameSaveState::SaveSlot;
	ULevelGameInstanceSubsystem::Get(GetGameInstance()->GetWorld())->SetCurrentPlayerStartTag(InPlayerStartTag);
}

FName USaveGameSubsystem::GetPlayerStartTag() const
{
	return CurrentSaveState.PlayerStartTag;
}

void USaveGameSubsystem::SaveSlotData(UAuraSaveGame* SaveGame) const
{
	if (UGameplayStatics::DoesSaveGameExist(SaveGame->SlotName, SaveGame->SlotIndex))
	{
		DeleteSlot(SaveGame->SlotName, SaveGame->SlotIndex);
	}
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SlotName, SaveGame->SlotIndex);
}

void USaveGameSubsystem::SaveInGameProgressData(UAuraSaveGame* SaveGame)
{
	CurrentSaveState.PlayerStartTag = SaveGame->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveGame, CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UAuraSaveGame* USaveGameSubsystem::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	UAuraSaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	else
	{
		SaveGameObject = CreateDefaultSaveData(SlotIndex, SlotName);
	}
	return SaveGameObject;
}

void USaveGameSubsystem::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UAuraSaveGame* SaveData = GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex))
	{
		if (!DestinationMapAssetName.IsEmpty())
		{
			SaveData->MapAssetName = DestinationMapAssetName;
			SaveData->MapName = ULevelGameInstanceSubsystem::Get(World)->GetMapNameFromMapAssetName(
				DestinationMapAssetName
			);
		}
		else
		{
			SaveData->MapAssetName = World->GetMapName();
			SaveData->MapName = WorldName;
		}
		if (!SaveData->HasMap(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveData->AddSavedMap(NewSavedMap);
		}
		if (AAuraGameState* GameState = AAuraGameState::Get(World))
		{
			GameState->ToSaveData(SaveData);
		}
		FSavedMap SavedMap = SaveData->GetSavedMapByMapName(WorldName);
		SavedMap.SavedActors.Empty();
		// TODO - PERF - consider a save "registry" that allows actors to register to be saved instead of iterating over everything in the world.
		for (FActorIterator ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			AActor* Actor = *ActorIterator;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
			{
				continue;
			}
			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.Transform = Actor->GetTransform();
			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;
			Actor->Serialize(Archive);
			SavedMap.SavedActors.AddUnique(SavedActor);
		}
		SaveData->ReplaceSavedMap(WorldName, SavedMap);
		UGameplayStatics::SaveGameToSlot(SaveData, SaveData->SlotName, SaveData->SlotIndex);
	}
}

void USaveGameSubsystem::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UGameplayStatics::DoesSaveGameExist(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex))
	{
		UAuraSaveGame* SaveData = GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
		const FSavedMap& SavedMap = SaveData->GetSavedMapByMapName(WorldName);
		if (AAuraGameState* GameState = AAuraGameState::Get(World))
		{
			GameState->FromSaveData(SaveData);
		}
		if (!SavedMap.IsValid())
		{
			UE_LOG(LogAura, Error, TEXT("[%s] Could not find saved map with name: %s"), *GetName(), *WorldName);
			return;
		}
		TMap<FName, FSavedActor> SavedActorMap;
		SavedMap.FillActorsByName(SavedActorMap);
		for (FActorIterator ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			AActor* Actor = *ActorIterator;
			if (!Actor->Implements<USaveInterface>())
			{
				continue;
			}
			if (SavedActorMap.Contains(Actor->GetFName()))
			{
				const FSavedActor& SavedActor = SavedActorMap[Actor->GetFName()];
				if (ISaveInterface::ShouldLoadTransform(Actor))
				{
					Actor->SetActorTransform(SavedActor.Transform);
					UE_LOG(
						LogAura,
						Warning,
						TEXT("[%s] Loading transform for actor: %s"),
						*GetName(),
						*Actor->GetName()
					);
				}
				FMemoryReader MemoryReader(SavedActor.Bytes);
				FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
				Archive.ArIsSaveGame = true;
				Actor->Serialize(Archive);
				ISaveInterface::Execute_LoadActor(Actor);
			}
		}
	}
}

void USaveGameSubsystem::DeleteSlot(const FString& SlotName, const int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

void USaveGameSubsystem::AutoSaveTransient()
{
	if (CurrentSaveState.SaveState == EAuraGameSaveState::Undefined)
	{
		CurrentSaveState.SlotIndex = AutoSaveSlot;
		CurrentSaveState.SlotName = AutoSaveName;
		CurrentSaveState.SaveState = EAuraGameSaveState::Transient;
		UAuraSaveGame* SaveGame = CreateDefaultSaveData(CurrentSaveState.SlotIndex, CurrentSaveState.SlotName);
		SaveSlotData(SaveGame);
	}
}
