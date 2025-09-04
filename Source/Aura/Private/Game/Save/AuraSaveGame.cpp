// Copyright Alien Shores


#include "Game/Save/AuraSaveGame.h"

#include "Aura/AuraLogChannels.h"

const FString UAuraSaveGame::DEFAULT_SAVE_SLOT_NAME = FString("Default");

UAuraSaveGame::UAuraSaveGame()
{
	SaveVersion = CURRENT_SAVE_VERSION;
	SaveSlotName = DEFAULT_SAVE_SLOT_NAME;
}

bool UAuraSaveGame::FindWorldSaveData(const FString WorldName, FWorldSaveData& WorldSaveData)
{
	for (const FWorldSaveData& CurrentWorldSaveData : WorldsData)
	{
		if (CurrentWorldSaveData.WorldName == WorldName)
		{
			WorldSaveData = CurrentWorldSaveData;
			return true;
		}
	}
	return false;
}

void UAuraSaveGame::GetOrCreateWorldSaveData(const FString WorldName, FWorldSaveData& WorldSaveData)
{
	if (!FindWorldSaveData(WorldName, WorldSaveData))
	{
		WorldSaveData.WorldName = WorldName;
		WorldsData.Add(WorldSaveData);
	}
}

void UAuraSaveGame::SetActorsData(const FString& WorldPathName, TArray<FActorSaveData> ActorsSaveData)
{
	for (FWorldSaveData& WorldSaveData : WorldsData)
	{
		if (WorldSaveData.WorldName == WorldPathName)
		{
			WorldSaveData.ActorsData = ActorsSaveData;
			return;
		}
	}
	UE_LOG(LogAura, Warning, TEXT("[%s] No world save data found for world: %s"), *GetName(), *WorldPathName);
}

void UAuraSaveGame::ClearAllData()
{
	MetaData = FMetaSaveData();
	GlobalData = FGlobalSaveData();
	WorldsData.Empty();
	bIsAutoSave = false;
}
