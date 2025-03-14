// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*Asset Tags*/)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UAuraAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

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
};
