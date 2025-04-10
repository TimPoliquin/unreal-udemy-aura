// Copyright Alien Shores


#include "Item/AuraItemInfo.h"

FAuraItemDefinition UAuraItemInfo::FindItemByName(const FString& ItemName) const
{
	for (const FAuraItemDefinition& ItemDefinition : ItemInfo)
	{
		if (ItemDefinition.ItemName == ItemName)
		{
			return ItemDefinition;
		}
	}
	return FAuraItemDefinition();
}

FAuraItemDefinition UAuraItemInfo::FindItemByItemType(const EAuraItemType ItemType) const
{
	for (const FAuraItemDefinition& ItemDefinition : ItemInfo)
	{
		if (ItemDefinition.ItemType == ItemType)
		{
			return ItemDefinition;
		}
	}
	return FAuraItemDefinition();
}
