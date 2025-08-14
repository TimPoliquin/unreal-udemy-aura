// Copyright Alien Shores


#include "Actor/AuraLockedInterface.h"


// Add default functionality here for any IAuraLockedInterface functions that are not pure virtual.
void IAuraLockedInterface::Unlock(UObject* Gate)
{
	if (IsValid(Gate) && Gate->Implements<UAuraLockedInterface>())
	{
		Execute_Unlock(Gate);
	}
}
