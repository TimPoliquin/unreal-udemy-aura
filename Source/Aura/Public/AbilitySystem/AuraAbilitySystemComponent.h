// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeChangeDelegates.h"
#include "AuraAbilitySystemTypes.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/)
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, FGameplayAbilitySpec&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityEquippedSignature, const FAuraEquipAbilityPayload&, EquipPayload);

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FActivatePassiveEffectSignature,
	const FGameplayTag& /* Ability Tag*/,
	const bool /*bActivate*/
)

DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbilitySignature, const FGameplayTag& /*Ability Tag*/);
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	FEffectAssetTags OnEffectAssetTagsDelegate;

	void AddCharacterAbilities(
		const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities,
		const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities
	);

	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	virtual void OnRep_ActivateAbilities() override;
	FAbilitiesGiven OnAbilitiesGivenDelegate;
	FORCEINLINE bool HasFiredOnAbilitiesGivenDelegate() const
	{
		return bAbilitiesGiven;
	}

	void ForEachAbility(const FForEachAbility& ForEachAbilityDelegate);
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	void ServerUpdateAbilityStatuses(const int32 Level);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);
	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag);
	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FAuraEquipAbilityPayload& EquipPayload);
	void ClearAbilitySlot(FGameplayAbilitySpec& AbilitySpec);
	void ClearAbilitiesUsingSlot(const FGameplayTag& SlotTag);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, FAuraAbilityDescription& OutDescription);


	FOnPlayerAbilityStatusChangedSignature OnPlayerLevelChangedDelegate;
	FAbilityEquippedSignature OnAbilityEquippedDelegate;
	FDeactivatePassiveAbilitySignature OnDeactivatePassiveAbilityDelegate;
	FActivatePassiveEffectSignature OnActivatePassiveEffectDelegate;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Client, Reliable)
	void Client_EffectApplied(
		UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec,
		FActiveGameplayEffectHandle ActiveEffectHandle
	);

private:
	bool bAbilitiesGiven = false;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(
		const int32 PlayerLevel,
		const TArray<FAbilityTagStatus>& AbilityStatuses
	);

	bool IsSlotEmpty(const FGameplayTag& SlotTag);
	FGameplayAbilitySpec* GetAbilitySpecWithSlot(const FGameplayTag& SlotTag);
	void AssignSlotTagToAbilitySpec(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag);
};
