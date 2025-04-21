// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayAbility.h"
#include "Interaction/FishingComponentInterface.h"
#include "AuraFishingReelAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFishingReelAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ReelIn(AActor* Player);

private:
	UFUNCTION()
	void OnFishingStateChanged(EFishingState FishingState);
};
