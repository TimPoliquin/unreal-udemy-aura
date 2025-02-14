// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Ability/AuraGameplayAbility.h"


void UAuraAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		this,
		&UAuraAbilitySystemComponent::Client_EffectApplied
	);
}

void UAuraAbilitySystemComponent::Client_EffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec,
	FActiveGameplayEffectHandle ActiveEffectHandle
)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	OnEffectAssetTags.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> StartupAbilities)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			for (FGameplayTag StartupTag : AuraAbility->GetStartupInputTag())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(StartupTag);
			}
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}
