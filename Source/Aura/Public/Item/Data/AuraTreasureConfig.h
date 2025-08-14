// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Utils/RandUtils.h"
#include "AuraTreasureConfig.generated.h"

class AAuraTreasurePickup;

USTRUCT(BlueprintType)
struct FTreasureConfigOption
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRandRange Range;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UStaticMesh>> Meshes;
};

USTRUCT(BlueprintType)
struct FTreasureConfig
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	int32 Value = 0.f;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UStaticMesh> Mesh;

	bool IsValid() const
	{
		return Value > 0.f && Mesh != nullptr;
	}
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraTreasureConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FTreasureConfig GetTreasureConfig(const float Value, const bool Randomize = true) const;
	TSubclassOf<AAuraTreasurePickup> GetTreasureClass() const { return TreasureClass; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FTreasureConfigOption> TreasureConfigs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraTreasurePickup> TreasureClass;
};
