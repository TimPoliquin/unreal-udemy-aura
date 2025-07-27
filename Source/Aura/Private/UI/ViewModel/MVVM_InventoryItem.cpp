// Copyright Alien Shores


#include "UI/ViewModel/MVVM_InventoryItem.h"

void UMVVM_InventoryItem::InitializeDependencies(AAuraCharacter* PlayerActor)
{
}

FGameplayTag UMVVM_InventoryItem::GetInventoryItemTag() const
{
	return InventoryItemTag;
}

void UMVVM_InventoryItem::SetInventoryItemTag(const FGameplayTag& InInventoryItemTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(InventoryItemTag, InInventoryItemTag);
}

int32 UMVVM_InventoryItem::GetQuantity() const
{
	return Quantity;
}

void UMVVM_InventoryItem::SetQuantity(const int32 InQuantity)
{
	UE_MVVM_SET_PROPERTY_VALUE(Quantity, InQuantity);
}

int32 UMVVM_InventoryItem::GetRow() const
{
	return Row;
}

void UMVVM_InventoryItem::SetRow(const int32 InRow)
{
	this->Row = InRow;
}

int32 UMVVM_InventoryItem::GetColumn() const
{
	return Column;
}

void UMVVM_InventoryItem::SetColumn(const int32 InColumn)
{
	this->Column = InColumn;
}
