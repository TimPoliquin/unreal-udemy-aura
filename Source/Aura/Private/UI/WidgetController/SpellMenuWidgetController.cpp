// Copyright Alien Shores


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnSpellPointsChanged(GetAuraPlayerState()->GetSpellPoints());
}


void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	GetAuraAbilitySystemComponent()->OnAbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
		{
			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);
	GetAuraPlayerState()->OnSpellPointsChangeDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnSpellPointsChanged
	);
}

int32 USpellMenuWidgetController::GetSpellPoints()
{
	return GetAuraPlayerState()->GetSpellPoints();
}

void USpellMenuWidgetController::OnSpellPointsChanged(const int32 SpellPoints)
{
	OnSpellMenuSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
