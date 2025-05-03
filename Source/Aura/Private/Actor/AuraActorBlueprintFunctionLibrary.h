// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraActorBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraActorBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SinusoidalMovement(
		AActor* Actor,
		float DeltaSeconds,
		float& RunningTime,
		float PeriodMultiplier,
		float Amplitude,
		const FVector& InitialLocation
	);

	static void DisableActor(AActor* Actor);
};
