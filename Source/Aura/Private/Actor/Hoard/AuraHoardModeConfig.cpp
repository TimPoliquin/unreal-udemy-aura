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
