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
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Beam")
	TObjectPtr<UAnimMontage> AbilityMontage;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "Events.Montage"))
	FGameplayTag EventTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag SoundCueTag;
	UPROPERTY(EditDefaultsOnly, Category="Beam", meta=(Categories = "GameplayCue"))
	FGameplayTag LoopCueTag;

	FVector HitLocation;


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
