// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	FORCEINLINE TArray<FGameplayTag> GetStartupInputTag() const
	{
		return StartupInputTags;
	}

	FORCEINLINE FScalableFloat GetPower() const
	{
		return Power;
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Power")
	FScalableFloat Power;

private:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TArray<FGameplayTag> StartupInputTags;
};
