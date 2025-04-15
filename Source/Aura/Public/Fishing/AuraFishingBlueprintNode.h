// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraFishTypes.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Utils/RandUtils.h"
#include "AuraFishingBlueprintNode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingEventSignature, AActor*, FishingActor);

USTRUCT(BlueprintType)
struct FAuraGoFishingParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	AActor* CameraTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AActor* PlayerTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	AActor* FishingTarget = nullptr;
	UPROPERTY(BlueprintReadWrite)
	UCurveFloat* CameraMovementCurve = nullptr;
	UPROPERTY(BlueprintReadWrite)
	FRandRange TimeToLure;
	UPROPERTY(BlueprintReadWrite)
	FRandRange LureToBiteTime;
	UPROPERTY(BlueprintReadWrite)
	FRandRange BiteToFleeTime;
};

/**
 * 
 */
UCLASS(BlueprintType, meta=(ExposedAsyncProxy = AsyncTask))
class AURA_API UAuraFishingBlueprintNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
		Category = "Flow Control"
	)
	static UAuraFishingBlueprintNode* GoFishing(
		UObject* WorldContextObject,
		AActor* Player,
		const FAuraGoFishingParams& Params
	);
	UFUNCTION(BlueprintCallable)
	void WaitForFishToBeLured();
	UFUNCTION(BlueprintCallable)
	void LureAndWaitForABite(const EFishType& FishType);
	UFUNCTION(BlueprintCallable)
	void BiteAndWaitForPlayerOrFlee();
	UFUNCTION(BlueprintCallable)
	void Flee();
	UFUNCTION(BlueprintCallable)
	void Reel();
	UFUNCTION(BlueprintCallable)
	void Catch();

	virtual void Activate() override;

protected:
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnCameraInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingCancelledDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnPlayerInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingRodEquippedDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingRodCastDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingLuredDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGoFishingEventSignature OnFishingBiteDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingFishHasFledDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingFishReelingDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingEventSignature OnFishingFishCaughtDelegate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EFishType ActiveFishType = EFishType::None;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	EFishState ActiveFishState = EFishState::None;

	UFUNCTION(BlueprintCallable)
	void End();

private:
	UPROPERTY()
	AActor* PlayerActor;
	UPROPERTY()
	FAuraGoFishingParams GoFishingParams;

	UPROPERTY()
	FTimerHandle PlayerMoveToTargetTimerHandle;
	FTimerDelegate PlayerMoveToTargetTimerDelegate;
	UPROPERTY()
	FTimerHandle FishInterestToLureTimerHandle;
	UPROPERTY()
	FTimerHandle FishLureToBiteTimerHandle;
	UPROPERTY()
	FTimerHandle FishBiteToFleeTimerHandle;

	FOnCameraMoveFinishedSignature OnCameraMoveFinishedDelegate;
	void MoveCameraToPosition();
	void MovePlayerToPosition();
	UFUNCTION()
	void OnCameraInPosition();
	UFUNCTION()
	void OnFishingRodCast();
	UFUNCTION()
	void OnFishingRodEquipped();
	UFUNCTION()
	void OnFishingStateChanged(EFishingState FishingState);
	void SetFishState(const EFishState& InFishState);
};
