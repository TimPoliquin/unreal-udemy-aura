﻿// Copyright Alien Shores


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

float IFishingComponentInterface::GetRarityMultiplier(const UObject* Actor, const FGameplayTag& Rarity)
{
	if (IsValid(Actor) && Actor->Implements<UFishingActorInterface>())
	{
		if (TScriptInterface<IFishingComponentInterface> FishingComponent = IFishingActorInterface::GetFishingComponent(
			Actor
		))
		{
			return FishingComponent->GetRarityMultiplier(Rarity);
		}
	}
	return 0.f;
}

bool IFishingComponentInterface::IsFishing(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UFishingActorInterface>())
	{
		if (TScriptInterface<IFishingComponentInterface> FishingComponent = IFishingActorInterface::GetFishingComponent(
			Actor
		))
		{
			return FishingComponent->IsFishing();
		}
	}
	return false;
}

FGameplayTagContainer IFishingComponentInterface::GetFishingTags(const UObject* Actor)
{
	if (IsValid(Actor) && Actor->Implements<UFishingActorInterface>())
	{
		if (TScriptInterface<IFishingComponentInterface> FishingComponent = IFishingActorInterface::GetFishingComponent(
			Actor
		))
		{
			return FishingComponent->GetFishingTags();
		}
	}
	return FGameplayTagContainer();
}
