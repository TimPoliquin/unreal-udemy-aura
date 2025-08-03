#pragma once

#include "CoreMinimal.h"
#include "AuraSpawnEventPayloadDefinitions.generated.h"

USTRUCT(BlueprintType)
struct FOnAuraActorTrackerCountChangedPayload
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AActor> Owner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PreviousCount = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 NewCount = 0;

	FOnAuraActorTrackerCountChangedPayload()
	{
		Owner = nullptr;
		PreviousCount = 0;
		NewCount = 0;
	}

	FOnAuraActorTrackerCountChangedPayload(AActor* InOwner, const int32 InPreviousCount, const int32 InNewCount)
	{
		Owner = InOwner;
		PreviousCount = InPreviousCount;
		NewCount = InNewCount;
	}

	bool IsEmpty() const
	{
		return NewCount == 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuraActorTrackerCountChangedDelegate, const FOnAuraActorTrackerCountChangedPayload&, Payload);

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
