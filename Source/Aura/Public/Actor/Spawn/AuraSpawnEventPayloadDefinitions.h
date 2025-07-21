#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.generated.h"

USTRUCT(BlueprintType)
struct FOnAuraActorTrackerCountChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PreviousCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NewCount = 0;

	FOnAuraActorTrackerCountChangedPayload()
	{
		PreviousCount = 0;
		NewCount = 0;
	}

	FOnAuraActorTrackerCountChangedPayload(const int32 InPreviousCount, const int32 InNewCount)
	{
		PreviousCount = InPreviousCount;
		NewCount = InNewCount;
	}

	bool IsEmpty() const
	{
		return NewCount == 0;
	}
};

UCLASS()
class AURA_API UAuraSpawnEventHelpers : public UObject
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsEmpty(const FOnAuraActorTrackerCountChangedPayload& InPayload)
	{
		return InPayload.IsEmpty();
	}
};
