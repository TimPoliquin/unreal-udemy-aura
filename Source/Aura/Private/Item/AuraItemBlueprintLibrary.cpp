// Copyright Alien Shores


#include "Item/AuraItemBlueprintLibrary.h"

#include "Aura/AuraLogChannels.h"
#include "Fishing/AuraFishInfo.h"
#include "Fishing/AuraFishTypes.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"

FAuraItemDefinition UAuraItemBlueprintLibrary::GetItemDefinitionByItemType(
	const UObject* WorldContextObject,
	const FGameplayTag& ItemType
)
{
	if (UAuraGameDataSubsystem* GameDataSubsystem = UAuraGameDataSubsystem::Get(WorldContextObject))
	{
		return GameDataSubsystem->FindItemDefinitionByItemTag(ItemType);
	}
	UE_LOG(LogAura, Error, TEXT("[%s] Attempted get item definition, but game mode was null!"), *FString("UAuraItemBlueprintLibrary::GetItemDefinitionByItemType"));
	return FAuraItemDefinition();
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
	UAuraGameDataSubsystem* GameDataSubsystem = UAuraGameDataSubsystem::Get(WorldContextObject);
	FAuraItemDefinition ItemDefinition = GameDataSubsystem->FindItemDefinitionByItemTag(FishType);
	FAuraFishDefinition FishDefinition = GameDataSubsystem->GetFishInfo()->GetFishDefinitionByFishType(FishType);
	FAuraFishCatch Catch;
	Catch.FishType = FishType;
	Catch.Description = ItemDefinition.ItemDescription;
	Catch.FishName = ItemDefinition.ItemName;
	Catch.Icon = FishDefinition.Icon;
	Catch.Size = FishDefinition.WeightRange.Value();
	return Catch;
}
