// Copyright Alien Shores


#include "Game/AuraSaveGame.h"

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
	for (FSavedMap& MapToReplace : SavedMaps)
	{
		if (MapToReplace.MapAssetName.Equals(InMapAssetName))
		{
			MapToReplace = SavedMap;
		}
	}
}
