// Copyright Alien Shores


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerState.h"
#include "Player/Progression/AuraProgressionComponent.h"
#include "Tags/AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	if (GetAuraAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddLambda([this]()
		{
			BroadcastAbilityInfo();
		});
	}
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		OnSpellPointsChanged(FAuraIntAttributeChangedPayload::CreateBroadcastPayload(FAuraGameplayTags::Get().Attributes_Progression_SpellPoints, ProgressionComponent->GetSpellPoints()));
	}
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
	if (UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		ProgressionComponent->OnSpellPointsChangeDelegate.AddDynamic(
			this,
			&USpellMenuWidgetController::OnSpellPointsChanged
		);
	}
}

int32 USpellMenuWidgetController::GetAvailableSpellPoints()
{
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		return ProgressionComponent->GetSpellPoints();
	}
	return -1;
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

void USpellMenuWidgetController::UnbindAll_Implementation(const UObject* BoundObject)
{
	Super::UnbindAll_Implementation(BoundObject);
	OnSpellMenuSpellPointsChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuPlayerLevelChangedDelegate.RemoveAll(BoundObject);
	OnSpellMenuSlotClearedDelegate.RemoveAll(BoundObject);
	OnAbilityAssigned.RemoveAll(BoundObject);
}


void USpellMenuWidgetController::OnSpellPointsChanged(const FAuraIntAttributeChangedPayload& Payload)
{
	OnSpellMenuSpellPointsChangedDelegate.Broadcast(Payload);
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
	OnSpellMenuPlayerLevelChangedDelegate.Broadcast(FAuraIntAttributeChangedPayload(FAuraGameplayTags::Get().Attributes_Progression_Level, Level, Level));
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
