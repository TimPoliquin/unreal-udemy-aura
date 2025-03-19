// Copyright Alien Shores


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "Tags/AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnSpellPointsChanged(GetAuraPlayerState()->GetSpellPoints());
}


void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	GetAuraAbilitySystemComponent()->OnPlayerLevelChangedDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnPlayerLevelChanged
	);
	GetAuraPlayerState()->OnSpellPointsChangeDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnSpellPointsChanged
	);
}

int32 USpellMenuWidgetController::GetAvailableSpellPoints()
{
	return GetAuraPlayerState()->GetSpellPoints();
}

FGameplayTag USpellMenuWidgetController::GetAbilityStatusTag(const FGameplayTag AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetAuraAbilitySystemComponent()->GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag StatusTag = UAuraAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec);
		return StatusTag;
	}
	return FAuraGameplayTags::Get().Abilities_Status_Locked;
}

bool USpellMenuWidgetController::HasAvailableSpellPoints()
{
	return GetAvailableSpellPoints() > 0;
}

bool USpellMenuWidgetController::CanEquipAbility(const FGameplayTag& AbilityTag)
{
	const FGameplayTag StatusTag = GetAbilityStatusTag(AbilityTag);
	FGameplayTagContainer EquippableStatuses;
	EquippableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Unlocked);
	EquippableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
	return StatusTag.MatchesAny(EquippableStatuses);
}

bool USpellMenuWidgetController::CanPurchaseAbility(const FGameplayTag& AbilityTag)
{
	if (HasAvailableSpellPoints())
	{
		const FGameplayTag StatusTag = GetAbilityStatusTag(AbilityTag);
		FGameplayTagContainer PurchasableStatuses;
		PurchasableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
		PurchasableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Unlocked);
		PurchasableStatuses.AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
		return StatusTag.MatchesAny(PurchasableStatuses);
	}
	return false;
}

void USpellMenuWidgetController::SpendPointOnAbility(const FGameplayTag& AbilityTag)
{
	GetAuraAbilitySystemComponent()->ServerSpendSpellPoint(AbilityTag);
}

FAuraAbilityDescription USpellMenuWidgetController::GetAbilityDescription(const FGameplayTag AbilityTag)
{
	FAuraAbilityDescription AbilityDescription;
	GetAuraAbilitySystemComponent()->GetDescriptionsByAbilityTag(AbilityTag, AbilityDescription);
	return AbilityDescription;
}

FGameplayTag USpellMenuWidgetController::GetAbilityType(const FGameplayTag& AbilityTag) const
{
	return AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
}


void USpellMenuWidgetController::OnSpellPointsChanged(const int32 SpellPoints)
{
	OnSpellMenuSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void USpellMenuWidgetController::OnPlayerLevelChanged(
	const int32 Level,
	const TArray<FAbilityTagStatus>& AbilityStatuses
)
{
	if (AbilityInfo)
	{
		for (const FAbilityTagStatus& AbilityStatus : AbilityStatuses)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityStatus.AbilityTag);
			Info.StatusTag = AbilityStatus.StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	}
	OnSpellMenuPlayerLevelChangedDelegate.Broadcast(Level);
}
