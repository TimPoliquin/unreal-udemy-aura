// Copyright Alien Shores


#include "Actor/AuraFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"


AAuraFireball::AAuraFireball()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAuraFireball::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraFireball, ExplosionDamageConfig);
}

bool AAuraFireball::IsWithinExplodeDistance() const
{
	return FVector::Distance(
		GetActorLocation(),
		GetReturnToLocation()
	) <= ExplodeDistance;
}

FVector AAuraFireball::GetReturnToLocation() const
{
	return IsValid(ReturnToActor)
		       ? ReturnToActor->GetActorLocation()
		       : InitialLocation;
}

void AAuraFireball::ExplodeOnTarget(
	AActor* Target
)
{
	if (!IsValid(Target))
	{
		return;
	}
	FDamageEffectParams ExplosionDamageEffectParams = UAuraAbilitySystemLibrary::MakeCustomDamageEffectParams(
		GetOwner(),
		Target,
		ExplosionDamageEffectClass,
		ExplosionDamageConfig,
		AbilityLevel,
		GetActorLocation()
	);
	UAuraAbilitySystemLibrary::ApplyDamageEffect(
		ExplosionDamageEffectParams
	);
}

void AAuraFireball::SetupExplosionConfig(
	const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
	const FAuraDamageConfig& InExplosionDamageConfig,
	const int32 InAbilityLevel
)
{
	ExplosionDamageEffectClass = InExplosionDamageEffectClass;
	ExplosionDamageConfig = InExplosionDamageConfig;
	AbilityLevel = InAbilityLevel;
}

void AAuraFireball::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		InitialLocation = GetActorLocation();
		ApexLocation = GetActorForwardVector() * TravelDistance + InitialLocation;
		StartOutgoingTimeline();
	}
}

void AAuraFireball::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!IsValidOverlap(OtherActor))
	{
		return;
	}
	if (HasAuthority())
	{
		// DEVNOTE - this was hack copied out of AuraProjectile. This probably should have been refactored to be configurable instead.
		if (UAbilitySystemComponent* OtherAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
			OtherActor
		))
		{
			DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.TargetAbilitySystemComponent = OtherAbilitySystem;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}
