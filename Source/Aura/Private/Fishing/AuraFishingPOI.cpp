// Copyright Alien Shores


#include "AuraFishingPOI.h"

#include "Aura/AuraLogChannels.h"
#include "Fishing/AuraFishInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/FishingComponentInterface.h"
#include "Utils/ArrayUtils.h"


// Sets default values
AAuraFishingPOI::AAuraFishingPOI()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

TArray<FWeightedFish> AAuraFishingPOI::GetCurrentlyAvailableFish(const AActor* Player) const
{
	TArray<FWeightedFish> AvailableFish;
	FGameplayTagContainer PlayerFishTags = IFishingComponentInterface::GetFishingTags(Player);
	FGameplayTagContainer CombinedFishTags;
	CombinedFishTags.AppendTags(PoolTags);
	CombinedFishTags.AppendTags(PlayerFishTags);
	if (const AAuraGameModeBase* GameMode = AAuraGameModeBase::GetAuraGameMode(this))
	{
		for (const FFishConfig& FishConfig : FishConfigs)
		{
			const FAuraFishDefinition& FishDefinition = GameMode->GetFishInfo()->GetFishDefinitionByFishType(
				FishConfig.FishType
			);
			if (CombinedFishTags.HasAll(FishDefinition.Tags))
			{
				const float BaseRarityMultiplier = GameMode->GetFishInfo()->GetFishRarityMultiplierByRarity(
					FishDefinition.Rarity
				);
				const float PlayerRarityMultiplier = IFishingComponentInterface::GetRarityMultiplier(
					Player,
					FishDefinition.Rarity
				);
				UE_LOG(
					LogAura,
					Warning,
					TEXT("[%s]: Rarity multiplier for fish [%s] - [%f] * [%f]"),
					*GetName(),
					*FishConfig.FishType.ToString(),
					BaseRarityMultiplier,
					PlayerRarityMultiplier
				);
				FWeightedFish WeightedFish;
				WeightedFish.FishType = FishConfig.FishType;
				WeightedFish.Weight = 100.f * (FishConfig.RarityMultiplier * BaseRarityMultiplier *
					PlayerRarityMultiplier);
				if (WeightedFish.Weight > 0)
				{
					AvailableFish.Add(WeightedFish);
				}
				else if (bDebug)
				{
					UE_LOG(
						LogAura,
						Warning,
						TEXT("[%s]: Fish has weight <= 0. [%s]"),
						*GetName(),
						*WeightedFish.FishType.ToString()
					)
				}
			}
		}
	}
	return AvailableFish;
}

FGameplayTag AAuraFishingPOI::PickFishType(const TArray<FWeightedFish>& AvailableFish) const
{
	float TotalWeight = 0.f;
	for (const auto& [FishType, Weight] : AvailableFish)
	{
		TotalWeight += Weight;
	}
	TArray<FWeightedFish> FishByWeight = AvailableFish;
	FishByWeight.Sort(
		[](const FWeightedFish& A, const FWeightedFish& B)
		{
			return A.Weight < B.Weight;
		}
	);
	const float Roll = FMath::RandRange(0.f, TotalWeight);
	if (bDebug)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s]: Player rolled [%f] for fish."), *GetName(), Roll);
	}
	float CumulativeWeight = 0.f;
	for (const auto& [FishType, Weight] : FishByWeight)
	{
		CumulativeWeight += Weight;
		if (bDebug)
		{
			UE_LOG(
				LogAura,
				Warning,
				TEXT("[%s]: Checking roll [%f] against weight [%f] (combined: [%f]"),
				*GetName(),
				Roll,
				Weight,
				CumulativeWeight
			);
		}
		if (Roll < CumulativeWeight)
		{
			if (bDebug)
			{
				UE_LOG(LogAura, Warning, TEXT("[%s]: Chose a fish! [%s]"), *GetName(), *FishType.ToString());
			}
			return FishType;
		}
	}
	return FGameplayTag::EmptyTag;
}
