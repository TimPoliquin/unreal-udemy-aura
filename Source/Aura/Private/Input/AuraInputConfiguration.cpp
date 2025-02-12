// Copyright Alien Shores


#include "Input/AuraInputConfiguration.h"

const UInputAction* UAuraInputConfiguration::FindAbilityInputActionForTag(
	const FGameplayTag& InputTag,
	const bool bLogNotFound
) const
{
	for (const FAuraInputAction& CurrentAction : AbilityInputActions)
	{
		if (CurrentAction.InputTag.MatchesTagExact(InputTag))
		{
			return CurrentAction.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to find InputAction for tag %s"), *InputTag.ToString());
	}
	return nullptr;
}
