// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemInterface.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"


// Add default functionality here for any IAuraAbilitySystemInterface functions that are not pure virtual.
int32 IAuraAbilitySystemInterface::GetCharacterLevel(const UObject* Character)
{
	if (IsValid(Character) && Character->Implements<UAuraAbilitySystemInterface>())
	{
		return Execute_GetCharacterLevel(Character);
	}
	return -1;
}

bool IAuraAbilitySystemInterface::IsAbilitySystemReady(AActor* Actor)
{
	if (const UAuraAbilitySystemComponent* AbilitySystemComponent = UAuraAbilitySystemLibrary::GetAuraAbilitySystemComponent(Actor))
	{
		return AbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate();
	}
	return false;
}
