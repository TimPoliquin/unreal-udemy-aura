// Copyright Alien Shores


#include "Game/Subsystem/Old_SaveGameManager.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameState.h"
#include "Game/Save/OLD_AuraSaveGame.h"
#include "Game/Subsystem/AuraLevelManager.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UOld_SaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogAura, Warning, TEXT("[%s] Initialize"), *GetName())
	if (bAutoSaveOnStart)
	{
		AutoSaveTransient();
	}
}

void UOld_SaveGameManager::Deinitialize()
{
	if (CurrentSaveState.SaveState == EAuraGameSaveState::Undefined || CurrentSaveState.SaveState ==
		EAuraGameSaveState::Transient)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Deleting save game: %s"), *GetName(), *CurrentSaveState.SlotName);
		UGameplayStatics::DeleteGameInSlot(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
	}
	Super::Deinitialize();
}

UOld_SaveGameManager* UOld_SaveGameManager::Get(
	const UObject* WorldContextObject
)
{
	if (IsValid(WorldContextObject))
	{
		return UGameplayStatics::GetGameInstance(WorldContextObject)->GetSubsystem<UOld_SaveGameManager>();
	}
	UE_LOG(LogAura, Error, TEXT("[%s] Invalid local player"), *FString("LocalPlayerSaveGameSubsystem::Get"));
	return nullptr;
}

UOLD_AuraSaveGame* UOld_SaveGameManager::GetInGameSaveData() const
{
	return GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UOLD_AuraSaveGame* UOld_SaveGameManager::CreateDefaultSaveData(
	const int32 SlotIndex,
	const FString& SlotName
) const
{
	const UObject* WorldContextObject = GetGameInstance()->GetWorld();
	const UAuraLevelManager* LevelGameInstanceSubsystem = UAuraLevelManager::Get(WorldContextObject);
	UOLD_AuraSaveGame* SaveGame = Cast<UOLD_AuraSaveGame>(
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

void UOld_SaveGameManager::InitializeSaveState(const UOLD_AuraSaveGame* SaveGame, const bool bIsTransient)
{
	InitializeSaveState(SaveGame->PlayerStartTag, SaveGame->SlotName, SaveGame->SlotIndex, bIsTransient);
}

void UOld_SaveGameManager::InitializeSaveState(
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
}

FName UOld_SaveGameManager::GetPlayerStartTag() const
{
	return CurrentSaveState.PlayerStartTag;
}

void UOld_SaveGameManager::SaveSlotData(UOLD_AuraSaveGame* SaveGame) const
{
	if (UGameplayStatics::DoesSaveGameExist(SaveGame->SlotName, SaveGame->SlotIndex))
	{
		DeleteSlot(SaveGame->SlotName, SaveGame->SlotIndex);
	}
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SlotName, SaveGame->SlotIndex);
}

void UOld_SaveGameManager::SaveInGameProgressData(UOLD_AuraSaveGame* SaveGame)
{
	CurrentSaveState.PlayerStartTag = SaveGame->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveGame, CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UOLD_AuraSaveGame* UOld_SaveGameManager::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	UOLD_AuraSaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = Cast<UOLD_AuraSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	else
	{
		SaveGameObject = CreateDefaultSaveData(SlotIndex, SlotName);
	}
	return SaveGameObject;
}

void UOld_SaveGameManager::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UOLD_AuraSaveGame* SaveData = GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex))
	{
		if (!DestinationMapAssetName.IsEmpty())
		{
			SaveData->MapAssetName = DestinationMapAssetName;
			SaveData->MapName = UAuraLevelManager::Get(World)->GetMapNameFromMapAssetName(
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

void UOld_SaveGameManager::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UGameplayStatics::DoesSaveGameExist(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex))
	{
		UOLD_AuraSaveGame* SaveData = GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
		const FSavedMap& SavedMap = SaveData->GetSavedMapByMapName(WorldName);
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

void UOld_SaveGameManager::DeleteSlot(const FString& SlotName, const int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

void UOld_SaveGameManager::AutoSaveTransient()
{
	if (CurrentSaveState.SaveState == EAuraGameSaveState::Undefined)
	{
		CurrentSaveState.SlotIndex = AutoSaveSlot;
		CurrentSaveState.SlotName = AutoSaveName;
		CurrentSaveState.SaveState = EAuraGameSaveState::Transient;
		UOLD_AuraSaveGame* SaveGame = CreateDefaultSaveData(CurrentSaveState.SlotIndex, CurrentSaveState.SlotName);
		SaveSlotData(SaveGame);
	}
}
