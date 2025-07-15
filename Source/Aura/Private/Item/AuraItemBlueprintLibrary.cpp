// Copyright Alien Shores


#include "Item/AuraItemBlueprintLibrary.h"

#include "Fishing/AuraFishInfo.h"
#include "Fishing/AuraFishTypes.h"
#include "Game/AuraGameModeBase.h"

FAuraItemDefinition UAuraItemBlueprintLibrary::GetItemDefinitionByItemType(
	const UObject* WorldContextObject,
	const FGameplayTag& ItemType
)
{
	return AAuraGameModeBase::GetAuraGameMode(WorldContextObject)->FindItemDefinitionByItemTag(ItemType);
}

FString UAuraItemBlueprintLibrary::GetItemNameByItemType(
	const UObject* WorldContextObject,
	const FGameplayTag& ItemType
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

UTexture2D* UAuraItemBlueprintLibrary::SubstituteMessageIcon(UTexture2D* MessageIcon, const FMessageSubstitutions& MessageSubstitutions)
{
	if (MessageSubstitutions.Icon != nullptr)
	{
		return MessageSubstitutions.Icon;
	}
	return MessageIcon;
}

FAuraFishCatch UAuraItemBlueprintLibrary::ToFishCatch(const UObject* WorldContextObject, const FGameplayTag& FishType)
{
	const AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(WorldContextObject);
	FAuraItemDefinition ItemDefinition = GameMode->FindItemDefinitionByItemTag(FishType);
	FAuraFishDefinition FishDefinition = GameMode->GetFishInfo()->GetFishDefinitionByFishType(FishType);
	FAuraFishCatch Catch;
	Catch.FishType = FishType;
	Catch.Description = ItemDefinition.ItemDescription;
	Catch.FishName = ItemDefinition.ItemName;
	Catch.Icon = FishDefinition.Icon;
	Catch.Size = FishDefinition.WeightRange.Value();
	return Catch;
}
