// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraHoardModeConfig.h"
#include "Actor/Spawn/AuraSpawnEventPayloadDefinitions.h"
#include "Components/ActorComponent.h"
#include "AuraHoardModeComponent.generated.h"

class AAuraPickupItemBase;

USTRUCT(BlueprintType)
struct FOnHoardModeRoundCompletePayload
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	int32 CompletedRound = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 NextRound = 0;
	UPROPERTY(BlueprintReadOnly)
	bool HasNextRound = false;
	UPROPERTY(BlueprintReadOnly)
	TArray<FHoardRoundRewards> Rewards;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoardModeRoundCompleteSignature, const FOnHoardModeRoundCompletePayload&, Payload);

class UAuraHoardModeConfig;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraHoardModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAuraHoardModeComponent();

	UFUNCTION(BlueprintCallable)
	void StartRound();
	UFUNCTION(BlueprintCallable)
	void EndRound();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasNextRound() const;
	UPROPERTY(BlueprintAssignable)
	FOnHoardModeRoundCompleteSignature OnHoardModeRoundComplete;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UAuraHoardModeConfig> HoardModeConfig;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(MustImplement="AuraSpawnInterface"))
	TArray<AActor*> SpawnPoints;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentRound = 0;

private:
	UPROPERTY()
	TArray<AActor*> ActiveSpawnPoints;
	UFUNCTION()
	void OnSpawnPointCountChanged(const FOnAuraActorTrackerCountChangedPayload& Payload);
	void PrepareNextRound();
};
