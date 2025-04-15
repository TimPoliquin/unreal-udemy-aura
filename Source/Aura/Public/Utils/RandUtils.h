#pragma once

#include "CoreMinimal.h"
#include "RandUtils.generated.h"

USTRUCT(BlueprintType)
struct FRandRange
{
	GENERATED_BODY()

	FRandRange()
	{
		Min = 0.f;
		Max = 0.f;
	}

	FRandRange(const float InMin, const float InMax)
	{
		Min = InMin;
		Max = InMax;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Min = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Max = 0.f;

	float Value() const
	{
		return FMath::FRandRange(Min, Max);
	}
};

UCLASS()
class AURA_API UAuraRandRangeBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static float RandRange(const FRandRange& Range)
	{
		return Range.Value();
	}
};
