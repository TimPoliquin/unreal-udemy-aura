// Copyright Alien Shores


#include "Game/Subsystem/AuraLevelTransition.h"

#include "Game/Save/AuraSaveGameManager.h"
#include "Game/Subsystem/AuraLevelManager.h"

void UAuraLevelTransition::Initialize(const FAuraLevelTransitionParams& Params)
{
	DestinationPlayerStartTag = Params.PlayerStartTag;
	bShouldLoad = Params.ShouldLoad();
	SaveSlot = Params.SaveSlot;
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* World)
	{
		if (bShouldLoad)
		{
			UAuraSaveGameManager::Get(World)->ApplySaveGame(SaveSlot, SlotIndex);
		}
		OnComplete.Broadcast(World);
	});
}

FName UAuraLevelTransition::GetDestinationPlayerStartTag() const
{
	return DestinationPlayerStartTag;
}
