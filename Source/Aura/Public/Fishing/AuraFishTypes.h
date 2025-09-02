#pragma once

#include "CoreMinimal.h"
#include "Item/AuraItemTypes.h"
#include "Utils/RandUtils.h"
#include "AuraFishTypes.generated.h"

UENUM(BlueprintType)
enum class EFishState : uint8
{
	None,
	Lured,
	Biting,
	Fighting,
	Caught,
	Fled,
};

USTRUCT(BlueprintType)
struct FAuraFishDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo", meta=(Categories="Item.Type.Fish"))
	FGameplayTag FishType = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TObjectPtr<const UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo", meta=(Categories="Fish.Rarity"))
	FGameplayTag Rarity = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo", meta=(Categories="Fish.Tag"))
	FGameplayTagContainer Tags = FGameplayTagContainer();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FRandRange WeightRange;
};

USTRUCT(BlueprintType)
struct FAuraFishRarity
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo", meta=(Categories="Fish.Rarity"))
	FGameplayTag Rarity = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	float RarityMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FAuraPlayerFishingLevelRarity
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	int32 Level = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo", meta=(Categories="Fish.Rarity"))
	TMap<FGameplayTag, float> RarityMultipliers;
};

UENUM(BlueprintType)
enum class EFishingState : uint8
{
	None,
	Preparing,
	Equipping,
	Equipped,
	Casting,
	Cast,
	Waiting,
	Lured,
	Biting,
	Reeling,
	Caught,
	Fled,
};

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishingStateChangedSignature, EFishingState, FishingState);
