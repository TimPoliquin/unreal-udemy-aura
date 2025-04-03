// Copyright Alien Shores


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "ActiveGameplayEffectHandle.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Utils/TagUtils.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = true;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
	ApplyToTags.Add(TAG_PLAYER);
	ApplyToTags.Add(TAG_ENEMY);
}

void AAuraEffectActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bSinusoidalMovement || bRotates)
	{
		const float SinePeriod = 2 * PI / SinePeriodMultiplier;
		RunningTime += DeltaSeconds;
		if (RunningTime > SinePeriod)
		{
			RunningTime -= SinePeriod;
		}
		Bob(DeltaSeconds);
	}
}


void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	if (bSinusoidalMovement)
	{
		InitialLocation = GetActorLocation();
	}
	if (bPlaySpawnEffect)
	{
		PlaySpawnEffect();
		if (SpawnSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation(), GetActorRotation());
		}
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
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
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation(), GetActorRotation());
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
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
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, const FGameplayEffectConfig& GameplayEffectConfig)
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
			Destroy();
		}
	}
}


void AAuraEffectActor::RemoveEffectsFromTarget(AActor* TargetActor)
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

void AAuraEffectActor::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
}

void AAuraEffectActor::Bob(const float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		SetActorRelativeRotation(UKismetMathLibrary::ComposeRotators(GetActorRotation(), DeltaRotation));
	}
	if (bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodMultiplier);
		SetActorLocation(InitialLocation + FVector(0.f, 0.f, Sine));
	}
}
