// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AbilitySystem/AuraAbilitySystemTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
