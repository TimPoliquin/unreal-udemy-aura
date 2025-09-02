// Copyright Alien Shores


#include "Item/Component/AuraEffectComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Aura/AuraLogChannels.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/TagUtils.h"


// Sets default values for this component's properties
UAuraEffectComponent::UAuraEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ApplyToTags.Add(TAG_PLAYER);
	ApplyToTags.Add(TAG_ENEMY);
}


bool UAuraEffectComponent::CheckPreRequisites(AActor* PickupActor) const
{
	return TagUtils::HasAnyTag(PickupActor, ApplyToTags);
}

void UAuraEffectComponent::OnOverlap(AActor* TargetActor)
{
	if (GameplayEffectConfigs.IsEmpty())
	{
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] No overlap effect configured for effect component!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (!CheckPreRequisites(TargetActor))
	{
		return;
	}
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (!GameplayEffectConfig.IsValid())
		{
			UE_LOG(LogAura, Warning, TEXT("[%s][%s] Invalid overlap effect configured for component"), *GetOwner()->GetName(), *GetName())
		}
		if (GameplayEffectConfig.IsApplyOnOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
	}
	OnPlayerOverlapStart.Broadcast(TargetActor);
}

void UAuraEffectComponent::OnEndOverlap(AActor* TargetActor)
{
	if (!TagUtils::HasAnyTag(TargetActor, ApplyToTags))
	{
		// effect does not apply to target.
		return;
	}
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (GameplayEffectConfig.IsApplyOnEndOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
		if (GameplayEffectConfig.IsRemoveOnEndOverlap())
		{
			RemoveEffectsFromTarget(TargetActor);
		}
	}
	OnPlayerOverlapEnd.Broadcast(TargetActor);
}

void UAuraEffectComponent::SetDefaults(
	const TArray<FGameplayEffectConfig>& InEffectConfig,
	const bool InDestroyOnEffectApplication,
	const TArray<FName>& InApplyToTags
)
{
	GameplayEffectConfigs = InEffectConfig;
	bDestroyOnEffectApplication = InDestroyOnEffectApplication;
	ApplyToTags = InApplyToTags;
}

bool UAuraEffectComponent::IsAffectingActor(AActor* Actor) const
{
	if (const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor))
	{
		for (const TPair HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value == AbilitySystemComponent)
			{
				return true;
			}
		}
	}
	return false;
}

void UAuraEffectComponent::ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig)
{
	if (!GameplayEffectConfig.GameplayEffectClass)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s:%s] No effect configured!"), *GetOwner()->GetName(), *GetName());
		return;
	}
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(
			GameplayEffectConfig.GameplayEffectClass,
			ActorLevel,
			EffectContextHandle
		);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(
			*EffectSpecHandle.Data.Get()
		);
		const bool bIsInfiniteEffect = UAuraAbilitySystemLibrary::IsInfiniteEffect(EffectSpecHandle);
		if (bIsInfiniteEffect && GameplayEffectConfig.IsRemoveOnEndOverlap())
		{
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystem);
		}
		if (bDestroyOnEffectApplication && !bIsInfiniteEffect)
		{
			GetWorld()->DestroyActor(GetOwner());
		}
	}
}

void UAuraEffectComponent::RemoveEffectsFromTarget(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		TargetActor
	))
	{
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (TargetAbilitySystem == HandlePair.Value)
			{
				TargetAbilitySystem->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
