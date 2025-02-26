// Copyright Alien Shores


#include "Interaction/CombatInterface.h"

#include "Utils/ArrayUtils.h"


// Add default functionality here for any ICombatInterface functions that are not pure virtual.
int32 ICombatInterface::GetCharacterLevel() const
{
	return 0;
}

int32 ICombatInterface::GetCharacterLevel(const AActor* Actor)
{
	if (const ICombatInterface* CombatInterface = Cast<ICombatInterface>(Actor))
	{
		return CombatInterface->GetCharacterLevel();
	}
	return -1;
}

bool ICombatInterface::IsAlive(const AActor* Actor)
{
	if (Actor->Implements<UCombatInterface>())
	{
		return !Execute_IsDead(Actor);
	}
	return false;
}


bool ICombatInterface::IsDead(const AActor* Actor)
{
	if (Actor->Implements<UCombatInterface>())
	{
		return Execute_IsDead(Actor);
	}
	return false;
}

TArray<FTaggedMontage> ICombatInterface::GetAttackMontages(const AActor* Actor)
{
	if (Actor->Implements<UCombatInterface>())
	{
		return Execute_GetAttackMontages(Actor);
	}
	return TArray<FTaggedMontage>();
}

TArray<FName> ICombatInterface::GetTargetTagsToIgnore(const AActor* Actor)
{
	if (Actor->Implements<UCombatInterface>())
	{
		return Execute_GetTargetTagsToIgnore(Actor);
	}
	return TArray<FName>();
}
