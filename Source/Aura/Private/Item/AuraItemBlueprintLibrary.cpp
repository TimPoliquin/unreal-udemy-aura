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

FString UAuraItemBlueprintLibrary::GetItemNameByItemType(
	const UObject* WorldContextObject,
	const EAuraItemType ItemType
)
{
	return GetItemDefinitionByItemType(WorldContextObject, ItemType).ItemName;
}

FString UAuraItemBlueprintLibrary::Substitute(const FString& Message, const FMessageSubstitutions& MessageSubstitutions)
{
	FString Result = Message;
	for (TPair Entry : MessageSubstitutions.Substitutions)
	{
		const FString Key = "{" + Entry.Key + "}";
		Result.ReplaceInline(*Key, *Entry.Value);
	}
	return Result;
}
