// Copyright Alien Shores


#include "Game/Save/AuraSaveGame.h"

void UAuraSaveGame::AddSavedAbility(const FSavedAbility& SaveAbility)
{
	SavedAbilities.AddUnique(SaveAbility);
}

void UAuraSaveGame::AddSavedMap(const FSavedMap& SaveMap)
{
	SavedMaps.AddUnique(SaveMap);
}

FSavedMap UAuraSaveGame::GetSavedMapByMapName(const FString& InMapAssetName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName.Equals(InMapAssetName))
		{
			return SavedMap;
		}
	}
	return FSavedMap();
}

bool UAuraSaveGame::HasMap(const FString& InMapAssetName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName.Equals(InMapAssetName))
		{
			return true;
		}
	}
	return false;
}

void UAuraSaveGame::ReplaceSavedMap(const FString& InMapAssetName, const FSavedMap& SavedMap)
{
	SavedMaps.RemoveAll([InMapAssetName](const FSavedMap& Map)
	{
		return Map.MapAssetName.Equals(InMapAssetName);
	});
	SavedMaps.AddUnique(SavedMap);
}
