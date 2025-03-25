// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/HighlightInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "Templates/Function.h"

void UAuraProjectileSpell::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// FGameplayTagContainer Tags;
	// CommitAbility(Handle, ActorInfo, ActivationInfo, &Tags);
}

void UAuraProjectileSpell::SpawnProjectile(
	const FVector& ProjectileTargetLocation,
	const AActor* HitActor,
	const FGameplayTag& SocketTag
)
{
	AActor* OwningActor = GetAvatarActorFromActorInfo();
	if (!OwningActor->HasAuthority())
	{
		// Do not execute on client - run on server only
		return;
	}
	check(ProjectileClass);
	const FVector SpawnLocation = GetProjectileSpawnLocation(SocketTag);
	const FRotator Rotation = GetProjectileSpawnRotation(ProjectileTargetLocation, SpawnLocation, HitActor);
	SpawnProjectile(SpawnLocation, Rotation);
}

FVector UAuraProjectileSpell::GetProjectileSpawnLocation(const FGameplayTag& SocketTag) const
{
	return ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag
	);
}

FRotator UAuraProjectileSpell::GetProjectileSpawnRotation(
	const FVector& TargetLocation,
	const FVector& SpawnLocation,
	const AActor* TargetActor
) const
{
	FRotator Rotation = (TargetLocation - SpawnLocation).Rotation();
	if (!IsValid(TargetActor) || !TargetActor->Implements<UHighlightInterface>())
	{
		// reset pitch if the hit actor is not something that can be targeted
		Rotation.Pitch = Pitch;
	}
	return Rotation;
}

AAuraProjectile* UAuraProjectileSpell::SpawnProjectile(
	const FVector& SpawnLocation,
	const FRotator& SpawnRotation,
	FOnSpawnProjectileFinishedSignature* BeforeFinishSpawning
) const
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	AAuraProjectile* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetAvatarActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	SpawnedProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	if (BeforeFinishSpawning)
	{
		BeforeFinishSpawning->ExecuteIfBound(SpawnedProjectile);
	}
	SpawnedProjectile->FinishSpawning(SpawnTransform);
	return SpawnedProjectile;
}

FGameplayEffectSpecHandle UAuraProjectileSpell::MakeDamageEffectSpecHandle(
	AActor* SourceObject,
	const FVector& TargetLocation
) const
{
	const UAbilitySystemComponent* SourceAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetAvatarActorFromActorInfo()
	);
	FGameplayEffectContextHandle ContextHandle = SourceAbilitySystem->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(SourceObject);
	// Add actors to the context
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(SourceObject);
	ContextHandle.AddActors(Actors);
	// Set hit result on the context
	FHitResult HitResult;
	HitResult.Location = TargetLocation;
	ContextHandle.AddHitResult(HitResult);
	// Create outgoing spec
	return SourceAbilitySystem->MakeOutgoingSpec(
		DamageEffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
}
