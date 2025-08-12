// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupItemInventory.h"
#include "AuraTreasurePickup.generated.h"

UCLASS()
class AURA_API AAuraTreasurePickup : public AAuraPickupItemInventory
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraTreasurePickup();

	static AAuraTreasurePickup* SpawnTreasure(const AActor* WorldContextObject, const FVector& Location, float Value);
};
