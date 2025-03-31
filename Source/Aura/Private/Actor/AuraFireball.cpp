// Copyright Alien Shores


#include "Actor/AuraFireball.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


AAuraFireball::AAuraFireball()
{
	PrimaryActorTick.bCanEverTick = true;
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
