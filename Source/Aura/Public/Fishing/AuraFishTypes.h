#pragma once

#include "CoreMinimal.h"
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
