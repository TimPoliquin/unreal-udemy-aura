// Copyright Alien Shores


#include "Interaction/FishingActorInterface.h"


// Add default functionality here for any IFishingActorInterface functions that are not pure virtual.
TScriptInterface<IFishingComponentInterface> IFishingActorInterface::GetFishingComponent(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UFishingActorInterface>())
	{
		return Execute_GetFishingComponent(Actor);
	}
	return nullptr;
}
