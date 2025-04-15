// Copyright Alien Shores


#include "Interaction/FishingComponentInterface.h"

#include "Interaction/FishingActorInterface.h"


// Add default functionality here for any IFishingComponentInterface functions that are not pure virtual.
void IFishingComponentInterface::FishStateChanged(const UObject* Actor, const EFishState& FishState)
{
	if (IsValid(Actor) && Actor->Implements<UFishingActorInterface>())
	{
		if (TScriptInterface<IFishingComponentInterface> FishingComponent = IFishingActorInterface::GetFishingComponent(
			Actor
		))
		{
			FishingComponent->FishStateChanged(FishState);
		}
	}
}
