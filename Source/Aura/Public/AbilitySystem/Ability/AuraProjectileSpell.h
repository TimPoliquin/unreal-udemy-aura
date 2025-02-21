// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AuraProjectileSpell.generated.h"

class AAuraProjectile;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class AURA_API UAuraProjectileSpell : public UAuraGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	UFUNCTION(BlueprintCallable, Category ="Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

private:
	/**
	 * Angle (y) for projectile to be fired.
	 * Defaults to 0 to allow projectile to travel parallel to the ground. 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float Pitch = 0.f;

	FGameplayEffectSpecHandle MakeDamageEffectSpecHandle(AActor* SourceObject, const FVector& TargetLocation) const;
};
