// Copyright Alien Shores


#include "Interaction/PlayerInterface.h"


// Add default functionality here for any IPlayerInterface functions that are not pure virtual.
void IPlayerInterface::AddToXP(AActor* Actor, int32 InXP)
{
	if (Actor->Implements<UPlayerInterface>())
	{
		Execute_AddToXP(Actor, InXP);
	}
}
