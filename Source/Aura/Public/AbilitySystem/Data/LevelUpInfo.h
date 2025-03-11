// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement;
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointRewards = 1;
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointRewards = 1;
};

/**
 * 
 */
UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraLevelUpInfo> LevelUpInfos;

	FAuraLevelUpInfo FindLevelUpInfoByXP(const int32 XP) const;
	int32 FindLevelByXP(const int32 XP) const;

	float GetLevelProgressPercentage(const int32 CurrentXP) const;
};
