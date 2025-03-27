// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FTaggedMontage;

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	UFUNCTION(BlueprintCallable)
	void DealDamage(AActor* TargetActor);

	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomAttackMontage() const;

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FAuraDamageConfig DamageConfig;

	int32 GetDamageAtLevel(const int32 AbilityLevel) const;

	float GetManaCost(const float InLevel = 1.f) const;
	float GetCooldown(const float InLevel = 1.f) const;

	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bAutoActivateAbilityTag = true;
	UPROPERTY(EditDefaultsOnly, Category = "Configuration")
	bool bAutoDeactivateAbilityTag = true;

	FGameplayTag GetDefaultAbilityTag() const;
};
