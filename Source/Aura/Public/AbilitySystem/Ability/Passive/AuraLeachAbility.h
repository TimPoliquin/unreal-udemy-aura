// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPassiveAbility.h"
#include "AuraLeachAbility.generated.h"

struct FOnAbilitySystemOutgoingDamagePayload;
/**
 * 
 */
UCLASS()
class AURA_API UAuraLeachAbility : public UAuraPassiveAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat LeachConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> LeachEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString LeachAttribute = FString("");
	UFUNCTION(BlueprintCallable)
	FActiveGameplayEffectHandle ApplyLeachEffect(const FOnAbilitySystemOutgoingDamagePayload& Payload);

private:
	float GetLeachAmountAtLevel(const int32 Level) const;
	float GetLeachMagnitude(const FOnAbilitySystemOutgoingDamagePayload& Payload) const;
};
