// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	TObjectPtr<UAnimMontage> AbilityMontage;
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	FGameplayTag EventTag;

	UFUNCTION()
	void OnInputRelease(float TimeHeld);
	UFUNCTION()
	void OnReceiveMouseData(const FGameplayAbilityTargetDataHandle& DataHandle);

	UFUNCTION()
	void OnMontageEventReceived(FGameplayEventData Payload);
	void ExecuteAbility(const FHitResult& HitResult);
	void SetMouseCursorVisible(const bool Visible) const;
	void SetMovementEnabled(const bool Enabled) const;
};
