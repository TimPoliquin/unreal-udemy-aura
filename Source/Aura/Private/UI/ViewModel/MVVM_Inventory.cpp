// Copyright Alien Shores


#include "UI/ViewModel/MVVM_Inventory.h"

FString UMVVM_Inventory::GetInventoryName() const
{
	return InventoryName;
}

void UMVVM_Inventory::SetInventoryName(const FString& InInventoryName)
{
	UE_MVVM_SET_PROPERTY_VALUE(InventoryName, InInventoryName);
}
