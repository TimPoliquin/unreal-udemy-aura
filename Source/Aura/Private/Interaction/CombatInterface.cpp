// Copyright Alien Shores


#include "Interaction/CombatInterface.h"


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
