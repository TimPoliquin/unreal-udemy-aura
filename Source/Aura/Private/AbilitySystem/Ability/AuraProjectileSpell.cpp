// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const AActor* HitActor)
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
		FAuraGameplayTags::Get().Combat_Socket_Weapon
	);
	FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
	if (HitActor == nullptr)
	{
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

	SpawnedProjectile->SetInstigator(Cast<APawn>(OwningActor));
	FGameplayEffectSpecHandle DamageSpecHandle = MakeDamageEffectSpecHandle(
		SpawnedProjectile,
		ProjectileTargetLocation
	);
	DamageSpecHandle.Data.Get()->GetContext().AddSourceObject(SpawnedProjectile);
	for (const FGameplayTag& DamageTag : FAuraGameplayTags::Get().GetDamageTypes())
	{
		float DamageValue = 0.f;
		if (DamageTypes.Contains(DamageTag))
		{
			DamageValue = DamageTypes[DamageTag].GetValueAtLevel(GetAbilityLevel());
		}
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			DamageSpecHandle,
			// Damage Type
			DamageTag,
			// Damage Value
			DamageValue
		);
	}
	SpawnedProjectile->DamageEffectSpecHandle = DamageSpecHandle;
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
