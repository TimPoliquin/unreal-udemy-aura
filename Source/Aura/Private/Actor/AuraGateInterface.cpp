// Copyright Alien Shores


#include "Actor/AuraGateInterface.h"


// Add default functionality here for any IAuraGateInterface functions that are not pure virtual.
void IAuraGateInterface::Unlock(UObject* Gate)
{
	if (IsValid(Gate) && Gate->Implements<UAuraGateInterface>())
	{
		Execute_Unlock(Gate);
	}
}
