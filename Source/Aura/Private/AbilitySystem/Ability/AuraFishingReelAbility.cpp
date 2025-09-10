// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraFishingReelAbility.h"

#include "Fishing/AuraFishingComponent.h"
#include "Interaction/FishingActorInterface.h"

void UAuraFishingReelAbility::ReelIn(AActor* Player)
{
	if (UAuraFishingComponent* FishingComponent = IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponent->OnFishingStateChangedDelegate.AddDynamic(
			this,
			&UAuraFishingReelAbility::OnFishingStateChanged
		);
		FishingComponent->Reel();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UAuraFishingReelAbility::OnFishingStateChanged(EFishingState FishingState)
{
	if (FishingState != EFishingState::Reeling)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
