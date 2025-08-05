// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "UObject/Interface.h"
#include "AuraSpawnInterface.generated.h"

struct FRandRange;
struct FEnemySpawnConfig;
class AAuraEnemy;
// This class does not need to be modified.
UINTERFACE()
class UAuraSpawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IAuraSpawnInterface
{
	GENERATED_BODY()

public:
	virtual FOnAuraActorTrackerCountChangedDelegate& GetOnCountChangedDelegate() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BeginSpawning(const int32 NumEnemies, const FRandRange& SpawnDelay, const TArray<FEnemySpawnConfig>& SpawnClasses);
};
