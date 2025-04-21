#pragma once

#include "CoreMinimal.h"
#include "Utils/RandUtils.h"
#include "AuraFishTypes.generated.h"

UENUM(BlueprintType)
enum class EFishType : uint8
{
	None,
	Fish_01,
	Fish_02,
	Fish_03,
	Fish_04
};

UENUM(BlueprintType)
enum class EFishRarity : uint8
{
	Common,
	Rare,
	Special
};

UENUM(BlueprintType)
enum class EFishTag: uint8
{
	Sun,
	Rain,
	Snow,
	Day,
	Night,
};

UENUM(BlueprintType)
enum class EFishState : uint8
{
	None,
	Lured,
	Biting,
	Fighting,
	Caught
};

USTRUCT(BlueprintType)
struct FAuraFishCatch
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishCatch")
	EFishType FishType = EFishType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString FishName = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishCatch")
	float Size = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString Description = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TObjectPtr<const UTexture2D> Icon = nullptr;
};

USTRUCT(BlueprintType)
struct FAuraFishDefinition
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	EFishType FishType = EFishType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString FishName = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FString Description = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TObjectPtr<const UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	EFishRarity Rarity = EFishRarity::Common;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TArray<EFishTag> Tags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	TSubclassOf<AActor> FishClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FishInfo")
	FRandRange WeightRange;

	FAuraFishCatch ToFishCatch() const
	{
		FAuraFishCatch FishCatch;
		FishCatch.FishType = FishType;
		FishCatch.FishName = FishName;
		FishCatch.Size = WeightRange.Value();
		FishCatch.Description = Description;
		FishCatch.Icon = Icon;
		return FishCatch;
	}
};

USTRUCT(BlueprintType)
struct FAuraFishRarity
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	EFishRarity Rarity = EFishRarity::Common;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	float RarityMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FAuraPlayerFishingLevelRarity
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	int32 Level = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FishInfo")
	TMap<EFishRarity, float> RarityMultipliers;
};
