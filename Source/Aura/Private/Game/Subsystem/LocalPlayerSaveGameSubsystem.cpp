// Copyright Alien Shores


#include "Game/Subsystem/LocalPlayerSaveGameSubsystem.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/Subsystem/LevelGameInstanceSubsystem.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void ULocalPlayerSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogAura, Warning, TEXT("[%s] Initialize"), *GetName())
	if (bAutoSaveOnStart)
	{
		AutoSaveTransient();
	}
}

void ULocalPlayerSaveGameSubsystem::Deinitialize()
{
	if (CurrentSaveState.SaveState == EAuraGameSaveState::Undefined || CurrentSaveState.SaveState ==
		EAuraGameSaveState::Transient)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Deleting save game: %s"), *GetName(), *CurrentSaveState.SlotName);
		UGameplayStatics::DeleteGameInSlot(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
	}
	Super::Deinitialize();
}

ULocalPlayerSaveGameSubsystem* ULocalPlayerSaveGameSubsystem::Get(
	const ULocalPlayer* LocalPlayer
)
{
	if (IsValid(LocalPlayer))
	{
		return LocalPlayer->GetSubsystem<ULocalPlayerSaveGameSubsystem>();
	}
	UE_LOG(LogAura, Error, TEXT("[%s] Invalid local player"), *FString("LocalPlayerSaveGameSubsystem::Get"));
	return nullptr;
}

UAuraSaveGame* ULocalPlayerSaveGameSubsystem::GetInGameSaveData() const
{
	return GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UAuraSaveGame* ULocalPlayerSaveGameSubsystem::CreateDefaultSaveData(
	const int32 SlotIndex,
	const FString& SlotName
) const
{
	UAuraSaveGame* SaveGame = Cast<UAuraSaveGame>(
		UGameplayStatics::CreateSaveGameObject(SaveGameClass)
	);
	SaveGame->SlotIndex = SlotIndex;
	SaveGame->SlotName = SlotName;
	SaveGame->PlayerName = SlotName;
	SaveGame->MapAssetName = ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->GetDefaultMapAssetName();
	SaveGame->MapName = ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->GetDefaultMapName();
	SaveGame->SaveSlotStatus = Taken;
	SaveGame->PlayerStartTag = ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->GetDefaultPlayerStartTag(
		SaveGame->MapName
	);
	SaveGame->PlayerLevel = ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->
		GetDefaultPlayerLevel(SaveGame->MapName);
	return SaveGame;
}

void ULocalPlayerSaveGameSubsystem::InitializeSaveState(const UAuraSaveGame* SaveGame, const bool bIsTransient)
{
	InitializeSaveState(SaveGame->PlayerStartTag, SaveGame->SlotName, SaveGame->SlotIndex, bIsTransient);
}

void ULocalPlayerSaveGameSubsystem::InitializeSaveState(
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
	ULevelGameInstanceSubsystem::Get(GetLocalPlayer())->SetCurrentPlayerStartTag(InPlayerStartTag);
}

FName ULocalPlayerSaveGameSubsystem::GetPlayerStartTag() const
{
	return CurrentSaveState.PlayerStartTag;
}

void ULocalPlayerSaveGameSubsystem::SaveSlotData(UAuraSaveGame* SaveGame) const
{
	if (UGameplayStatics::DoesSaveGameExist(SaveGame->SlotName, SaveGame->SlotIndex))
	{
		DeleteSlot(SaveGame->SlotName, SaveGame->SlotIndex);
	}
	UGameplayStatics::SaveGameToSlot(SaveGame, SaveGame->SlotName, SaveGame->SlotIndex);
}

void ULocalPlayerSaveGameSubsystem::SaveInGameProgressData(UAuraSaveGame* SaveGame)
{
	CurrentSaveState.PlayerStartTag = SaveGame->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveGame, CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
}

UAuraSaveGame* ULocalPlayerSaveGameSubsystem::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
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

void ULocalPlayerSaveGameSubsystem::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
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

void ULocalPlayerSaveGameSubsystem::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	if (UGameplayStatics::DoesSaveGameExist(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex))
	{
		UAuraSaveGame* SaveData = GetSaveSlotData(CurrentSaveState.SlotName, CurrentSaveState.SlotIndex);
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

void ULocalPlayerSaveGameSubsystem::DeleteSlot(const FString& SlotName, const int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

void ULocalPlayerSaveGameSubsystem::AutoSaveTransient()
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
