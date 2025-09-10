// Copyright Alien Shores


#include "Game/Save/AuraSaveGameManager.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameState.h"
#include "Game/Save/AuraSaveGame.h"
#include "Game/Save/SaveableInterface.h"
#include "Game/Subsystem/AuraLevelManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "Player/Progression/AuraProgressionComponent.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UAuraSaveGameManager* UAuraSaveGameManager::Get(const UObject* WorldContextObject)
{
	if (const UAuraGameInstance* GameInstance = UAuraGameInstance::Get(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UAuraSaveGameManager>();
	}
	return nullptr;
}

UAuraSaveGameManager::UAuraSaveGameManager()
{
	CurrentSaveSlotName = FString("");
}

void UAuraSaveGameManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
#if WITH_EDITOR
	if (bAllowDefaultSave)
	{
		CurrentSaveSlotName = UAuraSaveGame::DEFAULT_SAVE_SLOT_NAME;
		DeleteSaveGame(CurrentSaveSlotName);
	}
#endif
}

void UAuraSaveGameManager::Deinitialize()
{
#if WITH_EDITOR
	if (CurrentSaveSlotName == UAuraSaveGame::DEFAULT_SAVE_SLOT_NAME)
	{
		DeleteSaveGame(CurrentSaveSlotName);
	}
#endif
	CurrentSaveSlotName = FString("");
	Super::Deinitialize();
}

TArray<FString> UAuraSaveGameManager::GetAllSaveGameSlotNames() const
{
	TArray<FString> SaveSlots;

	// Get the save directory
	const FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("SaveGames/");

	// File extension for save files
	const FString FileExtension = TEXT(".sav");

	// Find all files with the .sav extension in the save directory
	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(SaveSlots, *SaveDirectory, *FileExtension);

	// Remove the file extensions to get just the slot names
	for (FString& Slot : SaveSlots)
	{
		Slot = FPaths::GetBaseFilename(Slot);
	}

	return SaveSlots;
}

FString UAuraSaveGameManager::AutoSave_LevelTransition(const FAuraSaveGameParams& SaveParams)
{
	if (CurrentSaveSlotName.IsEmpty())
	{
		UE_LOG(LogAura, Error, TEXT("[%s] Attempted to save game, but CurrentSaveSlotName is empty!"), *GetName())
		OnSaveCompleted.Broadcast(false);
		return FString("");
	}
	UAuraSaveGame* CurrentSaveData;
	if (DoesSaveGameExist(CurrentSaveSlotName, AutoSaveSlotIndex))
	{
		CurrentSaveData = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, AutoSaveSlotIndex));
	}
	else
	{
		CurrentSaveData = Cast<UAuraSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
	CurrentSaveData->bIsAutoSave = true;
	CurrentSaveData->MetaData.MapAssetName = SaveParams.DestinationMapName;
	CurrentSaveData->MetaData.PlayerStartTag = SaveParams.DestinationPlayerStartTag;
	CurrentSaveData->SaveSlotName = CurrentSaveSlotName;
	SaveGameData(CurrentSaveData);
	return CurrentSaveData->SaveSlotName;
}

UAuraSaveGame* UAuraSaveGameManager::CreateNewGame(const FString& PlayerName)
{
	if (DoesSaveGameExist(PlayerName, 0))
	{
		DeleteSaveGame(PlayerName);
	}
	const UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this);
	UAuraSaveGame* NewGame = Cast<UAuraSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	NewGame->SaveSlotName = PlayerName;
	NewGame->MetaData.PlayerName = PlayerName;
	NewGame->MetaData.MapAssetName = LevelManager->GetDefaultMapAssetName();
	NewGame->MetaData.MapDisplayName = LevelManager->GetDefaultMapName();
	NewGame->MetaData.PlayerStartTag = LevelManager->GetDefaultPlayerStartTag(NewGame->MetaData.MapAssetName);
	NewGame->MetaData.PlayerLevel = LevelManager->GetDefaultPlayerLevel(NewGame->MetaData.MapAssetName);

	// Perform the actual save
	UGameplayStatics::SaveGameToSlot(NewGame, NewGame->SaveSlotName, 0);

	return NewGame;
}

void UAuraSaveGameManager::SaveGame(const FAuraSaveGameParams& SaveParams)
{
	if (CurrentSaveSlotName.IsEmpty())
	{
		UE_LOG(LogAura, Error, TEXT("[%s] Attempted to save game, but CurrentSaveSlotName is empty!"), *GetName())
		OnSaveCompleted.Broadcast(false);
		return;
	}
	UAuraSaveGame* CurrentSaveData;
	if (DoesSaveGameExist(CurrentSaveSlotName, 0))
	{
		CurrentSaveData = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, 0));
	}
	else
	{
		CurrentSaveData = Cast<UAuraSaveGame>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}
	CurrentSaveData->bIsAutoSave = false;
	CurrentSaveData->MetaData.MapAssetName = SaveParams.DestinationMapName;
	if (const UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this))
	{
		CurrentSaveData->MetaData.MapDisplayName = LevelManager->GetMapNameFromMapAssetName(SaveParams.DestinationMapName);
	}
	CurrentSaveData->MetaData.PlayerStartTag = SaveParams.DestinationPlayerStartTag;
	CurrentSaveData->SaveSlotName = CurrentSaveSlotName;
	SaveGameData(CurrentSaveData);
}

void UAuraSaveGameManager::LoadGame(const FString& SlotName, const int32 SlotIndex)
{
	if (UAuraSaveGame* SaveGame = LoadSaveGameData(SlotName, SlotIndex))
	{
		FAuraLevelTransitionParams LevelTransitionParams;
		LevelTransitionParams.MapAssetName = SaveGame->MetaData.MapAssetName;
		LevelTransitionParams.PlayerStartTag = SaveGame->MetaData.PlayerStartTag;
		LevelTransitionParams.bShouldSave = false;
		LevelTransitionParams.bShouldLoad = true;
		LevelTransitionParams.SaveSlot = SlotName;
		UAuraLevelManager* LevelManager = UAuraLevelManager::Get(this);
		LevelManager->TransitionLevel(LevelTransitionParams);
		ApplySaveGame(SaveGame);
	}
}

void UAuraSaveGameManager::LoadMostRecentGame()
{
	if (bIsMostRecentSaveAutoSave)
	{
		LoadGame(CurrentSaveSlotName, AutoSaveSlotIndex);
	}
	else
	{
		LoadGame(CurrentSaveSlotName, 0);
	}
}

void UAuraSaveGameManager::SaveGameData(UAuraSaveGame* CurrentSaveData)
{
	// Save meta data
	SaveMetaData(CurrentSaveData);
	// Save global data
	SaveGlobalData(CurrentSaveData);
	// Save world data
	FWorldSaveData WorldSaveData;
	CurrentSaveData->GetOrCreateWorldSaveData(GetWorld()->GetPathName(), WorldSaveData);
	WorldSaveData.ClearAllData();
	SaveWorldData(CurrentSaveData, WorldSaveData);

	// Save actors data
	SaveActorsData(CurrentSaveData);

	// Perform the actual save
	const bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveData, CurrentSaveData->SaveSlotName, 0);

	// Broadcast completion event
	OnSaveCompleted.Broadcast(bSuccess);

	if (bSuccess)
	{
		bIsMostRecentSaveAutoSave = CurrentSaveData->bIsAutoSave;
		UE_LOG(LogTemp, Log, TEXT("Game saved successfully to slot: %s"), *CurrentSaveData->SaveSlotName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save game to slot: %s"), *CurrentSaveData->SaveSlotName);
	}
}

void UAuraSaveGameManager::ApplySaveGame(UAuraSaveGame* LoadedData)
{
	// Load meta data
	LoadMetaData(LoadedData);
	// Load global data
	LoadGlobalData(LoadedData);
	FWorldSaveData WorldSaveData;
	LoadedData->FindWorldSaveData(GetWorld()->GetPathName(), WorldSaveData);
	if (WorldSaveData.IsValid())
	{
		// Load world data
		LoadWorldData(LoadedData, WorldSaveData);
		// Load actors data
		LoadActorsData(LoadedData, WorldSaveData);
	}
	else
	{
		UE_LOG(LogAura, Log, TEXT("[%s] No world data found for %s - skipping world/actor data load"), *GetName(), *GetWorld()->GetPathName())
	}

	// Broadcast completion event
	OnLoadCompleted.Broadcast(true);

	UE_LOG(LogTemp, Log, TEXT("Game loaded successfully from slot: %s"), *LoadedData->SaveSlotName);
}

void UAuraSaveGameManager::ApplySaveGame(const FString& SaveSlot, const int32 SlotIndex)
{
	if (UAuraSaveGame* SaveGame = LoadSaveGameData(SaveSlot, SlotIndex))
	{
		ApplySaveGame(SaveGame);
	}
	else
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] No save game found with name: %s"), *GetName(), *SaveSlot);
	}
}

bool UAuraSaveGameManager::DoesSaveGameExist(const FString& SlotName, const int32 SlotIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex);
}

void UAuraSaveGameManager::DeleteSaveGame(const FString& SlotName)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	UGameplayStatics::DeleteGameInSlot(SlotName, 1);
}

FString UAuraSaveGameManager::GetCurrentSaveSlotName() const
{
	return CurrentSaveSlotName;
}

UAuraSaveGame* UAuraSaveGameManager::GetCurrentPrimarySaveGame() const
{
	if (!CurrentSaveSlotName.IsEmpty() && DoesSaveGameExist(CurrentSaveSlotName, 0))
	{
		return Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, 0));
	}
	return nullptr;
}

UAuraSaveGame* UAuraSaveGameManager::GetCurrentAutoSaveGame() const
{
	if (!CurrentSaveSlotName.IsEmpty() && DoesSaveGameExist(CurrentSaveSlotName, AutoSaveSlotIndex))
	{
		return Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSaveSlotName, AutoSaveSlotIndex));
	}
	return nullptr;
}

void UAuraSaveGameManager::SaveMetaData(UAuraSaveGame* SaveGame)
{
	SaveGame->MetaData.SaveTimestamp = FDateTime::UtcNow();
}

void UAuraSaveGameManager::SaveGlobalData(UAuraSaveGame* SaveGame)
{
	AAuraGameState* GameState = AAuraGameState::Get(GetWorld());
	FActorSaveData GameStateSaveData;
	SaveActorData(GameState, GameStateSaveData);
	SaveGame->MetaData.PlayerLevel = GameState->GetProgressionComponent()->GetCharacterLevel();
	SaveGame->GlobalData.GameStateSaveData = GameStateSaveData;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (const AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Iterator->Get()))
		{
			FActorSaveData PlayerSaveData;
			SaveActorData(PlayerController->GetPlayerState<AAuraPlayerState>(), PlayerSaveData);
			SaveGame->GlobalData.AddPlayerSaveData(PlayerSaveData);
		}
	}
}


void UAuraSaveGameManager::SaveWorldData(UAuraSaveGame* SaveData, FWorldSaveData& WorldSaveData)
{
	if (!SaveData || !GetWorld())
	{
		return;
	}

	FWorldSaveData WorldData;
	SaveData->FindWorldSaveData(GetWorld()->GetPathName(), WorldData);
	// Save basic world information
	// TODO !!!

	// Example: Save time of day (you'll need to implement this based on your game)
	// WorldData.TimeOfDay = YourTimeManager->GetTimeOfDay();

	// Save custom world properties
	// You can extend this to save any world-specific data your game needs
}

void UAuraSaveGameManager::SaveActorsData(UAuraSaveGame* SaveData)
{
	if (!SaveData || !GetWorld())
	{
		return;
	}
	TArray<FActorSaveData> ActorsSaveData;
	// Iterate through all actors in the world
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		if (!Actor || !IsValid(Actor))
		{
			continue;
		}

		// Check if actor implements the saveable interface
		if (Actor->Implements<USaveableInterface>())
		{
			if (ISaveableInterface::Execute_ShouldSave(Actor))
			{
				FActorSaveData ActorData;
				SaveActorData(Actor, ActorData);
				ActorsSaveData.Add(ActorData);
			}
		}
	}
	SaveData->SetActorsData(GetWorld()->GetPathName(), ActorsSaveData);
}

void UAuraSaveGameManager::SaveActorData(AActor* Actor, FActorSaveData& ActorData)
{
	if (!Actor)
	{
		return;
	}

	// Save basic actor information
	ActorData.ActorName = Actor->GetName();
	ActorData.ActorClass = Actor->GetClass()->GetName();
	ActorData.ActorTransform = Actor->GetActorTransform();

	// Save actor-specific data if it implements the saveable interface
	if (Actor->Implements<USaveableInterface>())
	{
		ActorData.bShouldAutoSpawn = ISaveableInterface::Execute_ShouldAutoSpawn(Actor);
		ActorData.ActorData = ISaveableInterface::Execute_SaveData(Actor);
		if (ActorData.ActorData.IsEmpty())
		{
			// Manually write the actor data
			FMemoryWriter Writer(ActorData.ActorData);
			FObjectAndNameAsStringProxyArchive Ar(Writer, true);
			Ar.ArIsSaveGame = true;
			Actor->Serialize(Ar);
		}
	}

	// Save component data
	for (UActorComponent* Component : Actor->GetComponents().Array())
	{
		if (Component && Component->Implements<USaveableInterface>())
		{
			if (ISaveableInterface::Execute_ShouldSave(Component))
			{
				FComponentSaveData ComponentData;
				SaveComponentData(Component, ComponentData);
				ActorData.ComponentsData.Add(ComponentData);
			}
		}
	}
}

void UAuraSaveGameManager::SaveComponentData(UActorComponent* Component, FComponentSaveData& ComponentData)
{
	if (!Component)
	{
		return;
	}

	ComponentData.ComponentName = Component->GetName();
	ComponentData.ComponentClass = Component->GetClass()->GetName();

	// Save component-specific data if it implements the saveable interface
	if (Component->Implements<USaveableInterface>())
	{
		ComponentData.ComponentData = ISaveableInterface::Execute_SaveData(Component);
	}
}

void UAuraSaveGameManager::LoadMetaData(const UAuraSaveGame* SaveData)
{
	// TODO - Is there actually any value in loading meta data?
}

void UAuraSaveGameManager::LoadGlobalData(UAuraSaveGame* SaveData)
{
	if (!SaveData || !GetWorld())
	{
		return;
	}
	if (AAuraGameState* GameState = AAuraGameState::Get(GetWorld()))
	{
		LoadActor(GameState, SaveData->GlobalData.GameStateSaveData);
	}
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		if (const AAuraPlayerController* PlayerController = Cast<AAuraPlayerController>(Iterator->Get()))
		{
			const AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
			const FActorSaveData& ActorSaveData = SaveData->GlobalData.GetPlayerSaveData(PlayerState);
			if (ActorSaveData.IsValid())
			{
				LoadActor(PlayerController->GetPlayerState<AAuraPlayerState>(), ActorSaveData);
			}
			else
			{
				UE_LOG(LogAura, Warning, TEXT("[%s] No saved data for player: %s"), *GetName(), *PlayerState->GetName());
			}
		}
	}
}


void UAuraSaveGameManager::LoadWorldData(const UAuraSaveGame* SaveData, const FWorldSaveData& WorldSaveData)
{
	if (!SaveData || !GetWorld())
	{
		return;
	}

	// Load world-specific data
	// Example: Set time of day
	// YourTimeManager->SetTimeOfDay(WorldData.TimeOfDay);

	// Process world properties
	for (const auto& Property : WorldSaveData.WorldProperties)
	{
		// Handle custom world properties
		UE_LOG(LogTemp, Log, TEXT("Loading world property: %s = %s"), *Property.Key, *Property.Value);
	}
}

void UAuraSaveGameManager::LoadActorsData(const UAuraSaveGame* SaveData, const FWorldSaveData& WorldSaveData)
{
	if (!SaveData || !GetWorld() || !WorldSaveData.IsValid())
	{
		return;
	}

	// Clear existing dynamic actors (optional - depends on your game design)
	// You might want to only clear specific types of actors

	// Load saved actors
	for (const FActorSaveData& ActorData : WorldSaveData.ActorsData)
	{
		if (ActorData.IsValid())
		{
			LoadActorData(ActorData);
		}
	}
}

AActor* UAuraSaveGameManager::LoadActorData(const FActorSaveData& ActorData)
{
	if (!GetWorld())
	{
		return nullptr;
	}

	// Try to find existing actor first
	AActor* ExistingActor = nullptr;
	for (TActorIterator<AActor> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		if (ActorIterator->GetName() == ActorData.ActorName)
		{
			ExistingActor = *ActorIterator;
			break;
		}
	}

	AActor* TargetActor = ExistingActor;
	// If actor doesn't exist, spawn it
	if (!TargetActor && ActorData.bShouldAutoSpawn)
	{
		if (UClass* ActorClass = FindObject<UClass>(ANY_PACKAGE, *ActorData.ActorClass))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Name = *ActorData.ActorName;
			TargetActor = GetWorld()->SpawnActor<AActor>(ActorClass, ActorData.ActorTransform, SpawnParams);
		}
	}
	if (TargetActor)
	{
		LoadActor(TargetActor, ActorData);
	}

	return TargetActor;
}


void UAuraSaveGameManager::LoadActor(AActor* TargetActor, const FActorSaveData& ActorData)
{
	if (TargetActor)
	{
		// Load actor data if it implements the saveable interface
		if (TargetActor->Implements<USaveableInterface>())
		{
			if (ISaveableInterface::Execute_ShouldLoadTransform(TargetActor))
			{
				TargetActor->SetActorTransform(ActorData.ActorTransform);
			}
			ISaveableInterface::Execute_LoadData(TargetActor, ActorData.ActorData);
		}

		// Load component data
		for (const FComponentSaveData& ComponentData : ActorData.ComponentsData)
		{
			if (!ComponentData.IsValid())
			{
				continue;
			}
			if (UActorComponent* Component = TargetActor->FindComponentByClass(FindObject<UClass>(ANY_PACKAGE, *ComponentData.ComponentClass)))
			{
				LoadComponentData(Component, ComponentData);
			}
		}
		if (TargetActor->HasActorBegunPlay() && TargetActor->Implements<USaveableInterface>())
		{
			ISaveableInterface::Execute_PostLoad(TargetActor);
		}
	}
}

void UAuraSaveGameManager::LoadComponentData(UActorComponent* Component, const FComponentSaveData& ComponentData)
{
	if (!Component)
	{
		return;
	}

	// Load component data if it implements the saveable interface
	if (Component->Implements<USaveableInterface>() && ComponentData.ComponentData.Num() > 0)
	{
		ISaveableInterface::Execute_LoadData(Component, ComponentData.ComponentData);
	}
}

UAuraSaveGame* UAuraSaveGameManager::LoadSaveGameData(const FString& SlotName, const int32 SlotIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Save game does not exist in slot: %s"), *SlotName);
		OnLoadCompleted.Broadcast(false);
		return nullptr;
	}

	UAuraSaveGame* LoadedData = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	if (!LoadedData)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load game from slot: %s"), *SlotName);
		OnLoadCompleted.Broadcast(false);
		return nullptr;
	}

	if (!LoadedData->bIsAutoSave)
	{
		CurrentSaveSlotName = LoadedData->SaveSlotName;
		bIsMostRecentSaveAutoSave = false;
	}
	return LoadedData;
}
