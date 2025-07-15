// Copyright Alien Shores


#include "Player/InventoryActorInterface.h"


// Add default functionality here for any IInventoryActorInterface functions that are not pure virtual.
UPlayerInventoryComponent* IInventoryActorInterface::GetInventoryComponent(const UObject* Object)
{
	if (IsValid(Object) && Object->Implements<UInventoryActorInterface>())
	{
		return Execute_GetInventoryComponent(Object);
	}
	return nullptr;
}
