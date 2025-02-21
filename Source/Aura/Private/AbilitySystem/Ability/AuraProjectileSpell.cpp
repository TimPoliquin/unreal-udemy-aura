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

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		// Do not execute on client - run on server only
		return;
	}
	check(ProjectileClass);
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		FVector SpawnLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SpawnLocation).Rotation();
		Rotation.Pitch = Pitch;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AActor* OwningActor = GetAvatarActorFromActorInfo();
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
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			DamageSpecHandle,
			FAuraGameplayTags::Get().Damage,
			Power.GetValueAtLevel(GetAbilityLevel())
		);
		SpawnedProjectile->DamageEffectSpecHandle = DamageSpecHandle;
		SpawnedProjectile->FinishSpawning(SpawnTransform);
	}
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
