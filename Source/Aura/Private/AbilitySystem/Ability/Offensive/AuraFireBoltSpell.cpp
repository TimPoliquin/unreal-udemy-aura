// Copyright Alien Shores


#include "AbilitySystem/Ability/AuraFireBoltSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/HighlightInterface.h"

FString UAuraFireBoltSpell::GetDescription_Implementation(const int32 AbilityLevel) const
{
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	const int32 Damage = GetDamageAtLevel(AbilityLevel);
	const FString Bolts = AbilityLevel == 1
		                      ? FString::Printf(TEXT("a bolt"))
		                      : FString::Printf(
			                      TEXT("%d bolts"),
			                      GetNumProjectiles(AbilityLevel)
		                      );
	return FString::Printf(
		TEXT(
			// Title
			"" RICH_TITLE("%s") "\n\n"
			// Details
			RICH_SMALL("Level: ") RICH_LEVEL("%d") "\n" // Level
			RICH_SMALL("Mana Cost: ") RICH_MANA_COST("%.1f") "\n" // Mana Cost
			RICH_SMALL("Cooldown: ") RICH_COOLDOWN("%.1f") "\n\n" // Cooldown
			// Description
			RICH_DEFAULT("Launches %s of fire, exploding on impact and dealing ") RICH_DAMAGE("%d")
			RICH_DEFAULT(
				"fire damage with a chance to burn."
			) "\n\n"
		),
		*GetAbilityName(),
		AbilityLevel,
		FMath::Abs(ManaCost),
		FMath::Abs(Cooldown),
		*Bolts,
		Damage
	);
}

void UAuraFireBoltSpell::SpawnProjectiles(
	const FVector& ProjectileTargetLocation,
	const FGameplayTag& SocketTag,
	AActor* Target
)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		return;
	}
	const FVector SpawnLocation = GetProjectileSpawnLocation(SocketTag);
	const FRotator Rotation = GetProjectileSpawnRotation(ProjectileTargetLocation, SpawnLocation, Target);
	const FVector Forward = Rotation.Vector();
	const TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(
		Forward,
		FVector::UpVector,
		ProjectileSpread,
		GetNumProjectiles(GetAbilityLevel())
	);
	FOnSpawnProjectileFinishedSignature OnSpawnFinish;
	OnSpawnFinish.BindLambda(
		[this, ProjectileTargetLocation, Target](AAuraProjectile* SpawnedProjectile)
		{
			if (IsValid(Target) && Target->Implements<UHighlightInterface>() && Target != GetAvatarActorFromActorInfo())
			{
				SpawnedProjectile->GetProjectileMovementComponent()->HomingTargetComponent = Target->GetRootComponent();
				if (ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(Target))
				{
					TargetCombatInterface->GetOnDeathDelegate().AddDynamic(
						SpawnedProjectile,
						&AAuraProjectile::OnTargetDead
					);
				}
			}
			else
			{
				SpawnedProjectile->HomingTargetSceneComponent = NewObject<USceneComponent>(
					USceneComponent::StaticClass()
				);
				SpawnedProjectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
				SpawnedProjectile->GetProjectileMovementComponent()->HomingTargetComponent = SpawnedProjectile->
					HomingTargetSceneComponent;
			}
			SpawnedProjectile->GetProjectileMovementComponent()->HomingAccelerationMagnitude = FMath::FRandRange(
				HomingAccelerationMin,
				HomingAccelerationMax
			);
			SpawnedProjectile->GetProjectileMovementComponent()->bIsHomingProjectile = true;
		}
	);

	for (const FRotator& ProjectileRotation : Rotations)
	{
		SpawnProjectile(
			SpawnLocation,
			ProjectileRotation,
			&OnSpawnFinish
		);
	}
}

int32 UAuraFireBoltSpell::GetNumProjectiles(const int32 AbilityLevel) const
{
	return FMath::Min(AbilityLevel, MaxNumProjectiles);
}
