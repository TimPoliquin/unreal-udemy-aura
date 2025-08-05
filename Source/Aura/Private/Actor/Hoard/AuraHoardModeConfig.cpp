// Copyright Alien Shores


#include "Actor/Hoard/AuraHoardModeConfig.h"

FHoardRoundConfig UAuraHoardModeConfig::GetRoundConfig(const int32 RoundNum) const
{
	if (RoundConfigs.IsValidIndex(RoundNum))
	{
		return RoundConfigs[RoundNum];
	}
	return FHoardRoundConfig();
}

bool UAuraHoardModeConfig::HasConfigurationForRound(const int32 RoundNum) const
{
	return RoundConfigs.IsValidIndex(RoundNum);
}

TArray<FHoardRoundRewards> UAuraHoardModeConfig::GetRewards(const int32 RoundNum) const
{
	return GetRoundConfig(RoundNum).Rewards;
}

int32 UAuraHoardModeConfig::GetNumberOfRewardsToSpawn(TArray<FHoardRoundRewards> Rewards)
{
	int32 NumberOfRewardsToSpawn = 0;
	for (const auto& [Reward, Count] : Rewards)
	{
		NumberOfRewardsToSpawn += Count;
	}
	return NumberOfRewardsToSpawn;
}
