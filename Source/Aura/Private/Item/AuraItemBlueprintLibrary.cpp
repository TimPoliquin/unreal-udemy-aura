// Copyright Alien Shores


#include "Item/AuraItemBlueprintLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Item/AuraItemInfo.h"

FAuraItemDefinition UAuraItemBlueprintLibrary::GetItemDefinitionByItemType(
	const UObject* WorldContextObject,
	const EAuraItemType ItemType
)
{
	return AAuraGameModeBase::GetAuraGameMode(WorldContextObject)->GetItemInfo()->FindItemByItemType(ItemType);
}
