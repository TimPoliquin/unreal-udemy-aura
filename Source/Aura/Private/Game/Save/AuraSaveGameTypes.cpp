#include "Game/Save/AuraSaveGameTypes.h"

FActorSaveData FGlobalSaveData::GetPlayerSaveData(const AAuraPlayerState* PlayerState)
{
	FActorSaveData* Match = PlayerSaveData.FindByPredicate([PlayerState](const FActorSaveData& SaveData)
	{
		return SaveData.ActorName.Equals(PlayerState->GetName());
	});
	if (Match)
	{
		return *Match;
	}
	return FActorSaveData();
}

void FGlobalSaveData::AddPlayerSaveData(const FActorSaveData& ActorSaveData)
{
	for (int32 Idx = 0; Idx < PlayerSaveData.Num(); Idx++)
	{
		if (PlayerSaveData[Idx].ActorName.Equals(ActorSaveData.ActorName))
		{
			PlayerSaveData[Idx] = ActorSaveData;
			return;
		}
	}
	PlayerSaveData.Add(ActorSaveData);
}
