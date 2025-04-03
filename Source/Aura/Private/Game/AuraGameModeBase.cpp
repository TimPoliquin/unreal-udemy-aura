// Copyright Alien Shores


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlot)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), LoadSlot->GetSlotIndex()))
	{
		DeleteSlot(LoadSlot->GetLoadSlotName(), LoadSlot->GetSlotIndex());
	}
	UAuraSaveGame* SaveGame = Cast<UAuraSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
	SaveGame->SlotIndex = LoadSlot->GetSlotIndex();
	SaveGame->SlotName = LoadSlot->GetLoadSlotName();
	SaveGame->PlayerName = LoadSlot->GetPlayerName();
	SaveGame->MapAssetName = LoadSlot->GetMapAssetName();
	SaveGame->MapName = LoadSlot->GetMapName();
	SaveGame->SaveSlotStatus = LoadSlot->GetLoadSlotStatus();
	SaveGame->PlayerStartTag = LoadSlot->GetPlayerStartTag();
	SaveGame->PlayerLevel = LoadSlot->GetPlayerLevel();
	UGameplayStatics::SaveGameToSlot(SaveGame, LoadSlot->GetLoadSlotName(), LoadSlot->GetSlotIndex());
}

void AAuraGameModeBase::SaveInGameProgressData(UAuraSaveGame* SaveGame)
{
	UAuraGameInstance* GameInstance = GetAuraGameInstance();
	GameInstance->PlayerStartTag = SaveGame->PlayerStartTag;
	UGameplayStatics::SaveGameToSlot(SaveGame, GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

UAuraSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	UAuraSaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = Cast<UAuraSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	else
	{
		SaveGameObject = Cast<UAuraSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
		SaveGameObject->SlotName = SlotName;
		SaveGameObject->SlotIndex = SlotIndex;
	}
	return SaveGameObject;
}

void AAuraGameModeBase::LoadMap(const UMVVM_LoadSlot* LoadSlot)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, MapsByName.FindChecked(LoadSlot->GetMapName()));
}

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	UAuraGameInstance* AuraGameInstance = GetAuraGameInstance();
	if (UAuraSaveGame* SaveData = GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		if (!DestinationMapAssetName.IsEmpty())
		{
			SaveData->MapAssetName = DestinationMapAssetName;
			SaveData->MapName = GetMapNameFromMapAssetName(DestinationMapAssetName);
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

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	UAuraGameInstance* AuraGameInstance = GetAuraGameInstance();
	if (UGameplayStatics::DoesSaveGameExist(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex))
	{
		UAuraSaveGame* SaveData = GetSaveSlotData(AuraGameInstance->LoadSlotName, AuraGameInstance->LoadSlotIndex);
		for (FActorIterator ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			AActor* Actor = *ActorIterator;
			if (!Actor->Implements<USaveInterface>())
			{
				continue;
			}
			// TODO - PERF - this should be a map or something to prevent N^2 operations
			for (FSavedActor SavedActor : SaveData->GetSavedMapByMapName(WorldName).SavedActors)
			{
				if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
				{
					Actor->SetActorTransform(SavedActor.Transform);
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

void AAuraGameModeBase::DeleteSlot(const FString& SlotName, const int32 SlotIndex)
{
	UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
}

FString AAuraGameModeBase::GetDefaultMapName() const
{
	return DefaultMapName;
}

FString AAuraGameModeBase::GetDefaultMapAssetName() const
{
	return DefaultMap.ToSoftObjectPath().GetAssetName();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if (Actors.Num() > 0)
	{
		AActor* SelectedActor = Actors[0];
		for (AActor* Actor : Actors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

UAuraGameInstance* AAuraGameModeBase::GetAuraGameInstance() const
{
	return Cast<UAuraGameInstance>(GetGameInstance());
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	MapsByName.Add(DefaultMapName, DefaultMap);
}

int32 AAuraGameModeBase::GetDefaultPlayerLevel() const
{
	return DefaultPlayerLevel;
}

void AAuraGameModeBase::SetDefaultPlayerLevel(const int32 InDefaultPlayerLevel)
{
	this->DefaultPlayerLevel = InDefaultPlayerLevel;
}

void AAuraGameModeBase::PlayerDied(ACharacter* PlayerCharacter)
{
	UAuraSaveGame* SaveData = GetInGameSaveData();
	if (!IsValid(SaveData))
	{
		return;
	}
	UGameplayStatics::OpenLevel(PlayerCharacter, FName(SaveData->MapAssetName));
}

FName AAuraGameModeBase::GetDefaultPlayerStartTag() const
{
	return DefaultPlayerStartTag;
}

void AAuraGameModeBase::SetDefaultPlayerStartTag(const FName& InDefaultPlayerStartTag)
{
	this->DefaultPlayerStartTag = InDefaultPlayerStartTag;
}

UAuraSaveGame* AAuraGameModeBase::GetInGameSaveData() const
{
	UAuraGameInstance* GameInstance = GetAuraGameInstance();
	return GetSaveSlotData(GameInstance->LoadSlotName, GameInstance->LoadSlotIndex);
}

AAuraGameModeBase* AAuraGameModeBase::GetAuraGameMode(const UObject* WorldContextObject)
{
	return Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (auto& Map : MapsByName)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName().Equals(MapAssetName))
		{
			return Map.Key;
		}
	}
	return FString("");
}
