// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/HighlightInterface.h"
#include "Tags/AuraGameplayTags.h"

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
	FVector SpawnLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		OwningActor,
		SocketTag
	);
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
	if (!HitActor || !HitActor->Implements<UHighlightInterface>())
	{
		// reset pitch if the hit actor is not something that can be targeted
		Rotation.Pitch = Pitch;
	}
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AAuraProjectile* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		OwningActor,
		Cast<APawn>(OwningActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	SpawnedProjectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	SpawnedProjectile->FinishSpawning(SpawnTransform);
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
