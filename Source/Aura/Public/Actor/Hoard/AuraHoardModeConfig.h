// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/Spawn/AuraEnemySpawnConfig.h"
#include "Engine/DataAsset.h"
#include "Item/Pickup/AuraPickupItemBase.h"
#include "Utils/RandUtils.h"
#include "AuraHoardModeConfig.generated.h"

struct FAuraSpawnParams;
class AAuraSpawnTunnel;
class AAuraEnemy;

USTRUCT(BlueprintType)
struct FHoardRoundRewards
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraPickupItemBase> RewardClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Count = 1;
};

USTRUCT(BlueprintType)
struct FHoardRoundConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumSpawnPoints = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumEnemiesToSpawn = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRandRange SpawnDelay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FEnemySpawnConfig> EnemyConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FHoardRoundRewards> Rewards;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraHoardModeConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FHoardRoundConfig GetRoundConfig(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasConfigurationForRound(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<FHoardRoundRewards> GetRewards(const int32 RoundNum) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetNumberOfRewardsToSpawn(TArray<FHoardRoundRewards> Rewards);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FHoardRoundConfig> RoundConfigs;
};
