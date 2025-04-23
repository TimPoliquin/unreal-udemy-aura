// Copyright Alien Shores


#include "Fishing/AuraFishInfo.h"

FAuraFishDefinition UFishInfo::GetFishDefinitionByFishType(const FGameplayTag& FishType) const
{
	const FAuraFishDefinition* Match = FishInfos.FindByPredicate(
		[FishType](const FAuraFishDefinition& FishInfo)
		{
			return FishInfo.FishType == FishType;
		}
	);
	if (Match)
	{
		return *Match;
	}
	return FAuraFishDefinition();
}

float UFishInfo::GetFishRarityMultiplierByRarity(const FGameplayTag& Rarity) const
{
	const FAuraFishRarity* Match = FishRarityInfos.FindByPredicate(
		[Rarity](const FAuraFishRarity& FishRarity)
		{
			return FishRarity.Rarity == Rarity;
		}
	);
	return Match
		       ? Match->RarityMultiplier
		       : 0.f;
}

float UFishInfo::GetFishRarityMultiplierByFishType(const FGameplayTag& FishType) const
{
	return GetFishRarityMultiplierByRarity(GetFishDefinitionByFishType(FishType).Rarity);
}

float UFishInfo::GetFishRarityMultiplierByPlayerFishingLevel(
	const int32& PlayerFishingLevel,
	const FGameplayTag& Rarity
) const
{
	const FAuraPlayerFishingLevelRarity* PlayerLevelRarity = PlayerFishingLevelRarityInfos.FindByPredicate(
		[PlayerFishingLevel, Rarity](const FAuraPlayerFishingLevelRarity& PlayerLevelRarityInfo)
		{
			return PlayerLevelRarityInfo.Level == PlayerFishingLevel && PlayerLevelRarityInfo.RarityMultipliers.
				Contains(Rarity);
		}
	);
	if (PlayerLevelRarity)
	{
		return PlayerLevelRarity->RarityMultipliers[Rarity];
	}
	return 1.f;
}
