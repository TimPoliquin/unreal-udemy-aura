// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpRewards
{
	GENERATED_BODY()

	int32 AttributePoints;
	int32 SpellPoints;

	FAuraLevelUpRewards()
	{
		AttributePoints = 0;
		SpellPoints = 0;
	}

	FAuraLevelUpRewards(const int32 InAttributePoints, const int32 InSpellPoints)
	{
		AttributePoints = InAttributePoints;
		SpellPoints = InSpellPoints;
	}
};

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

	FAuraLevelUpRewards GetRewards() const
	{
		return FAuraLevelUpRewards(AttributePointRewards, SpellPointRewards);
	}
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
	FAuraLevelUpRewards GetRewardsByLevel(const int32 InLevel) const;

	float GetLevelProgressPercentage(const int32 CurrentXP) const;
};
