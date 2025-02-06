// Copyright Alien Shores


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "ActiveGameplayEffectHandle.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	for (auto GameplayEffectConfig : GameplayEffectConfigs)
	{
		if (GameplayEffectConfig.IsApplyOnOverlap())
		{
			ApplyEffectToTarget(TargetActor, GameplayEffectConfig);
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
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
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig)
{
	check(GameplayEffectConfig.GameplayEffectClass);
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystem->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilitySystem->MakeOutgoingSpec(GameplayEffectConfig.GameplayEffectClass, 1.0f, EffectContextHandle);
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetAbilitySystem->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		bool bIsInfiniteEffect = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite; 
		if (bIsInfiniteEffect && GameplayEffectConfig.IsRemoveOnEndOverlap())
		{
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetAbilitySystem);
		}
		if (bDestroyOnEffectApplication)
		{
			Destroy();
		}
	}
}

void AAuraEffectActor::RemoveEffectsFromTarget(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
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



