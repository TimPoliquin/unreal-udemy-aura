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
	GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddDynamic(
		this,
		&USpellMenuWidgetController::OnAbilityEquipped
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
	return UAuraAbilitySystemLibrary::GetStatusTagByAbilityTag(GetAuraAbilitySystemComponent(), AbilityTag);
}

bool USpellMenuWidgetController::HasAvailableSpellPoints()
{
	return GetAvailableSpellPoints() > 0;
}

bool USpellMenuWidgetController::CanEquipAbility(const FGameplayTag& AbilityTag)
{
	return UAuraAbilitySystemLibrary::CanEquipAbility(GetAuraAbilitySystemComponent(), AbilityTag);
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

FGameplayTag USpellMenuWidgetController::GetAbilityTypeTag(const FGameplayTag& AbilityTag) const
{
	return AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
}

FGameplayTag USpellMenuWidgetController::GetAbilityInputTag(const FGameplayTag AbilityTag)
{
	return UAuraAbilitySystemLibrary::GetInputTagByAbilityTag(GetAuraAbilitySystemComponent(), AbilityTag);
}

void USpellMenuWidgetController::EquipAbility(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& SlotTag,
	const FGameplayTag& SelectedAbilityTypeTag
)
{
	const FGameplayTag AbilityType = GetAbilityTypeTag(AbilityTag);
	const FGameplayTag StatusTag = GetAbilityStatusTag(AbilityTag);
	const FGameplayTag CurrentInputTag = GetAbilityInputTag(AbilityTag);
	if (!SelectedAbilityTypeTag.MatchesTagExact(AbilityType))
	{
		UE_LOG(
			LogAura,
			Warning,
			TEXT("Attempted to assign an ability to an invalid slot. [%s] x-> [%s]"),
			*AbilityTag.ToString(),
			*SlotTag.ToString()
		)
		return;
	}
	GetAuraAbilitySystemComponent()->ServerEquipAbility(
		AbilityTag,
		SlotTag
	);
}

void USpellMenuWidgetController::UnbindAll(const UObject* BoundObject)
{
	OnSpellMenuSpellPointsChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuPlayerLevelChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuSlotClearedDelegate.RemoveAll(BoundObject);
	OnAbilityAssigned.RemoveAll(BoundObject);
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

void USpellMenuWidgetController::OnAbilityEquipped(const FAuraEquipAbilityPayload& EquipPayload)
{
	// clear the previously occupied slot
	OnSpellMenuSlotClearedDelegate.Broadcast(EquipPayload.PreviousSlotTag);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(EquipPayload.AbilityTag);
	Info.StatusTag = EquipPayload.StatusTag;
	Info.InputTag = EquipPayload.SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
	OnAbilityAssigned.Broadcast(EquipPayload.AbilityTag);
}
