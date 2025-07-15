// Copyright Alien Shores


#include "Item/Component/AuraEffectComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/TagUtils.h"


// Sets default values for this component's properties
UAuraEffectComponent::UAuraEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ApplyToTags.Add(TAG_PLAYER);
	ApplyToTags.Add(TAG_ENEMY);
}


void UAuraEffectComponent::OnOverlap(AActor* TargetActor)
{
	if (!TagUtils::HasAnyTag(TargetActor, ApplyToTags))
	{
		// effect does not apply to target.
		return;
	}
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (GameplayEffectConfig.IsApplyOnOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
	}
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
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

void UAuraEffectComponent::SetDefaults(const TArray<FGameplayEffectConfig>& InEffectConfig, const bool InDestroyOnEffectApplication, const TArray<FName>& InApplyToTags)
{
	GameplayEffectConfigs = InEffectConfig;
	bDestroyOnEffectApplication = InDestroyOnEffectApplication;
	ApplyToTags = InApplyToTags;
}

void UAuraEffectComponent::ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig)
{
	check(GameplayEffectConfig.GameplayEffectClass);
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
