// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraDamageGameplayAbility.h"
#include "AuraArcaneShardsSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraArcaneShardsSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription_Implementation(const int32 AbilityLevel) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetNumShards(const int32 AbilityLevel) const
	{
		return FMath::Min(AbilityLevel, MaxNumShards);
	}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxNumShards = 11;
};
