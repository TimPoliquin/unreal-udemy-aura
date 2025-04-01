// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "AuraFireball.generated.h"

UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AAuraFireball();

	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsWithinExplodeDistance() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetReturnToLocation() const;

	UFUNCTION(BlueprintCallable, Category="Fireball")
	void ExplodeOnTarget(
		AActor* Target
	);

	void SetupExplosionConfig(
		const TSubclassOf<UGameplayEffect>& InExplosionDamageEffectClass,
		const FAuraDamageConfig& InExplosionDamageConfig,
		int32
		InAbilityLevel
	);

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;


	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float TravelDistance;
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float ExplodeDistance = 250.f;
	UPROPERTY(BlueprintReadOnly)
	FVector InitialLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector ApexLocation;

private:
	UPROPERTY()
	TSubclassOf<UGameplayEffect> ExplosionDamageEffectClass;
	UPROPERTY(Replicated)
	FAuraDamageConfig ExplosionDamageConfig;
	int32 AbilityLevel;
};
