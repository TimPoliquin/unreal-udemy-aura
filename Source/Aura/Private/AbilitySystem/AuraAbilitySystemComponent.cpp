// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"


void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& ForEachAbilityDelegate)
{
	// This locks the ability system abilities for the scope of this function call.
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!ForEachAbilityDelegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in [%hs]"), __FUNCTION__);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (IPlayerInterface::GetAttributePoints(GetAvatarActor()) > 0)
	{
		ServerUpgradeAttribute(AttributeTag);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	IPlayerInterface::SpendAttributePoints(GetAvatarActor(), 1);
}

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
	OnEffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities,
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities
)
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
	for (const TSubclassOf PassiveAbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(PassiveAbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
	// NOTE: This is client-side only! OnRep_ActivateAbilities handles server-side.
	bAbilitiesGiven = true;
	OnAbilitiesGivenDelegate.Broadcast(this);
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

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bAbilitiesGiven)
	{
		bAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast(this);
	}
}
