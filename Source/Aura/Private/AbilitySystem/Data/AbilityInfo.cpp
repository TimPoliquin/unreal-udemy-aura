// Copyright Alien Shores


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag AbilityTag, bool bLogNotFound) const
{
	for (const FAuraAbilityInfo& AbilityInfo : AbilityInformation)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return AbilityInfo;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(
			LogAura,
			Error,
			TEXT("Unable to find ability by tag [%s] on Ability Info [%s]"),
			*AbilityTag.ToString(),
			*GetNameSafe(this)
		);
	}
	return FAuraAbilityInfo();
}
