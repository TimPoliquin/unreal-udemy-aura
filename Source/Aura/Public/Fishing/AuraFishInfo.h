// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraFishTypes.h"
#include "Engine/DataAsset.h"
#include "AuraFishInfo.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UFishInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FAuraFishDefinition GetFishDefinitionByFishType(const EFishType& FishType) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByRarity(const EFishRarity& Rarity) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByFishType(const EFishType& FishType) const;
	UFUNCTION(BlueprintCallable)
	float GetFishRarityMultiplierByPlayerFishingLevel(const int32& PlayerFishingLevel, const EFishRarity& Rarity) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FAuraFishDefinition> FishInfos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FAuraFishRarity> FishRarityInfos;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<FAuraPlayerFishingLevelRarity> PlayerFishingLevelRarityInfos;
};
