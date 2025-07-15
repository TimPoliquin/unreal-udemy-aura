// Copyright Alien Shores


#include "Item/Data/AuraItemInfo.h"
#include "Item/AuraItemTypes.h"

#include "Aura/AuraLogChannels.h"

FAuraItemDefinition UAuraItemInfo::FindItemByItemType(const FGameplayTag& ItemType) const
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

void UAuraItemInfo::AddToMap(TMap<FGameplayTag, FAuraItemDefinition>& ItemDefinitionMap) const
{
	for (const FAuraItemDefinition& ItemDefinition : ItemInfo)
	{
		if (ItemDefinitionMap.Contains(ItemDefinition.ItemType))
		{
			UE_LOG(LogAura, Warning, TEXT("[%s] Duplicate item definition detected: %s"), *GetName(), *ItemDefinition.ItemType.ToString());
		}
		else
		{
			ItemDefinitionMap.Add(ItemDefinition.ItemType, ItemDefinition);
		}
	}
}
