// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectileSpell.h"
#include "AuraFireBoltSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBoltSpell : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* Target);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float ProjectileSpread = 90.f;
	UPROPERTY(EditDefaultsOnly, Category ="Projectile")
	int32 MaxNumProjectiles;
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMin = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category = "FireBolt")
	float HomingAccelerationMax = 3200.f;

private:
	int32 GetNumProjectiles(const int32 AbilityLevel) const;
};
