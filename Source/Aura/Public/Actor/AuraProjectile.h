// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

struct FGameplayEffectSpecHandle;
class UProjectileMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class UAudioComponent;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAuraProjectile();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true), Replicated)
	FDamageEffectParams DamageEffectParams;

	UProjectileMovementComponent* GetProjectileMovementComponent() const;
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;
	UPROPERTY(EditDefaultsOnly)
	bool bShouldDestroyOnTargetDeath = true;

	UFUNCTION()
	void OnTargetDead(AActor* DeadActor);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Projectile")
	FVector GetImpactDirection(const AActor* HitActor) const;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	bool IsValidOverlap(const AActor* OtherActor) const;
	UFUNCTION(BlueprintCallable)
	void PlayImpactEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> TravelSound;
	UPROPERTY()
	TObjectPtr<UAudioComponent> TravelSoundComponent;
	bool bHit = false;
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 10.f;
};
