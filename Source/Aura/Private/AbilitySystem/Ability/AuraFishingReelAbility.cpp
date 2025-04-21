// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraFishingReelAbility.h"

#include "Interaction/FishingActorInterface.h"
#include "Interaction/FishingComponentInterface.h"

void UAuraFishingReelAbility::ReelIn(AActor* Player)
{
	if (const TScriptInterface<IFishingComponentInterface> FishingComponentInterface =
		IFishingActorInterface::GetFishingComponent(Player))
	{
		FishingComponentInterface->GetOnFishingStateChangedDelegate().AddDynamic(
			this,
			&UAuraFishingReelAbility::OnFishingStateChanged
		);
		FishingComponentInterface->Reel();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UAuraFishingReelAbility::OnFishingStateChanged(EFishingState FishingState)
{
	switch (FishingState)
	{
	case EFishingState::None:
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		break;
	case EFishingState::Caught:
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		break;
	default:
		// not sure yet
		break;
	}
}
