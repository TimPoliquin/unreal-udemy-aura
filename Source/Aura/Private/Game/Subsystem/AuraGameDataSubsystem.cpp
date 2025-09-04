// Copyright Alien Shores


#include "Game/Subsystem/AuraGameDataSubsystem.h"

#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Item/Data/AuraItemInfo.h"
#include "Kismet/GameplayStatics.h"

void UAuraGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeItemDefinitions();
}

UAuraGameDataSubsystem* UAuraGameDataSubsystem::Get(const UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = IsValid(WorldContextObject)
		                                        ? UGameplayStatics::GetGameInstance(WorldContextObject)
		                                        : nullptr)
	{
		return GameInstance->GetSubsystem<UAuraGameDataSubsystem>();
	}
	return nullptr;
}

FAuraItemDefinition UAuraGameDataSubsystem::FindItemDefinitionByItemTag(const FGameplayTag& ItemTag)
{
	if (ItemDefinitions.Contains(ItemTag))
	{
		return ItemDefinitions[ItemTag];
	}
	return FAuraItemDefinition();
}

FGameplayTag UAuraGameDataSubsystem::GetDefaultItemPickupMessageTag() const
{
	return DefaultItemPickupMessageTag;
}

FGameplayTag UAuraGameDataSubsystem::GetDefaultItemUseMessageTag() const
{
	return DefaultItemUsedMessageTag;
}


float UAuraGameDataSubsystem::GetXPToNextLevelPercentage(const float XP) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UAuraGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->GetLevelProgressPercentage(XP);
}

int32 UAuraGameDataSubsystem::FindLevelByXP(const int32 InXP) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UAuraGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->FindLevelByXP(InXP);
}

FAuraLevelUpRewards UAuraGameDataSubsystem::GetLevelUpRewards(int32 CurrentLevel) const
{
	checkf(LevelUpInfo, TEXT("LevelUpInfo not set on UAuraGameDataSubsystem - this must be set in the Blueprint"))
	return LevelUpInfo->GetRewardsByLevel(CurrentLevel);
}

void UAuraGameDataSubsystem::InitializeItemDefinitions()
{
	if (!ItemDefinitions.IsEmpty())
	{
		return;
	}
	for (const UAuraItemInfo* ItemDefinitionSet : ItemInfos)
	{
		ItemDefinitionSet->AddToMap(ItemDefinitions);
	}
}
