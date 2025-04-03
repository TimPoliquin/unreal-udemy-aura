// Copyright Alien Shores


#include "Character/EnemyInterface.h"


// Add default functionality here for any IEnemyInterface functions that are not pure virtual.
bool IEnemyInterface::IsEnemyActor(const UObject* Actor)
{
	return IsValid(Actor) && Actor->Implements<UEnemyInterface>();
}
