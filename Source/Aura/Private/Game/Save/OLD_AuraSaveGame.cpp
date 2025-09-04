// Copyright Alien Shores


#include "Game/Save/OLD_AuraSaveGame.h"

void UOLD_AuraSaveGame::AddSavedAbility(const FSavedAbility& SaveAbility)
{
	SavedAbilities.AddUnique(SaveAbility);
}

void UOLD_AuraSaveGame::AddSavedMap(const FSavedMap& SaveMap)
{
	SavedMaps.AddUnique(SaveMap);
}

FSavedMap UOLD_AuraSaveGame::GetSavedMapByMapName(const FString& InMapAssetName)
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

bool UOLD_AuraSaveGame::HasMap(const FString& InMapAssetName)
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

void UOLD_AuraSaveGame::ReplaceSavedMap(const FString& InMapAssetName, const FSavedMap& SavedMap)
{
	SavedMaps.RemoveAll([InMapAssetName](const FSavedMap& Map)
	{
		return Map.MapAssetName.Equals(InMapAssetName);
	});
	SavedMaps.AddUnique(SavedMap);
}
