// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "Actor/AuraProjectile.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
class UGameplayEffect;

DECLARE_DELEGATE_OneParam(FOnSpawnProjectileFinishedSignature, AAuraProjectile*);


/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AAuraProjectile> ProjectileClass;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION(BlueprintCallable, Category ="Projectile")
	void SpawnProjectile(
		const FVector& ProjectileTargetLocation,
		const AActor* HitActor,
		const FGameplayTag& SocketTag
	);

	FVector GetProjectileSpawnLocation(const FGameplayTag& SocketTag) const;
	FRotator GetProjectileSpawnRotation(
		const FVector& TargetLocation,
		const FVector& SpawnLocation,
		const AActor* TargetActor
	) const;

	AAuraProjectile* SpawnProjectile(
		const FVector& SpawnLocation,
		const FRotator& SpawnRotation,
		FOnSpawnProjectileFinishedSignature* BeforeFinishSpawning = nullptr
	) const;

	/**
	 * Angle (y) for projectile to be fired.
	 * Defaults to 0 to allow projectile to travel parallel to the ground. 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Pitch = 0.f;

private:
	FGameplayEffectSpecHandle MakeDamageEffectSpecHandle(AActor* SourceObject, const FVector& TargetLocation) const;
};
