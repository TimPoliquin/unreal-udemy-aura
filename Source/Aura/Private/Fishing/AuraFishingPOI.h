// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraPOI.h"
#include "Fishing/AuraFishTypes.h"
#include "Utils/RandUtils.h"
#include "AuraFishingPOI.generated.h"

enum class EFishTag : uint8;
enum class EFishType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishBiteSignature, const EFishType&, FishType);

USTRUCT(BlueprintType)
struct FFishConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EFishType FishType = EFishType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RarityMultiplier = 1.0f;
};

USTRUCT(BlueprintType)
struct FWeightedFish
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	EFishType FishType = EFishType::None;
	UPROPERTY(BlueprintReadOnly)
	float Weight = 1.f;
};


UCLASS()
class AURA_API AAuraFishingPOI : public AAuraPOI
{
	GENERATED_BODY()

public:
	AAuraFishingPOI();

	UFUNCTION(BlueprintCallable)
	TArray<FWeightedFish> GetCurrentlyAvailableFish(const AActor* Player) const;
	UFUNCTION(BlueprintCallable)
	EFishType PickFishType(const TArray<FWeightedFish>& AvailableFish) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	TArray<FFishConfig> FishConfigs;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	TArray<EFishTag> PoolTags;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange InterestToLureTime = FRandRange(5.f, 10.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange LureToBiteTime = FRandRange(5.f, 10.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing")
	FRandRange BiteToFleeTime = FRandRange(1.f, 7.f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fishing|Debug")
	bool bDebug = false;
};
