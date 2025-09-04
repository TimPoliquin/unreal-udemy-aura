// Copyright Alien Shores


#include "Game/Subsystem/AuraLevelTransition.h"

void UAuraLevelTransition::Initialize(const FName& InDestinationPlayerStartTag)
{
	DestinationPlayerStartTag = InDestinationPlayerStartTag;
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(this, [this](UWorld* World)
	{
		OnComplete.Broadcast(World);
	});
}

FName UAuraLevelTransition::GetDestinationPlayerStartTag() const
{
	return DestinationPlayerStartTag;
}
