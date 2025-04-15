// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraFishTypes.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AuraFishingBlueprintNode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingCancelledSignature, AActor*, FishingActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingPlayerInPositionSignature, AActor*, FishingActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingCameraInPositionSignature, AActor*, FishingActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingRodEquippedSignature, AActor*, FishingActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoFishingRodCastSignature, AActor*, FishingActor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FGoFishingFishBiteSignature,
	AActor*,
	FishingActor
);

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

	virtual void Activate() override;

protected:
	UPROPERTY(BlueprintAssignable)
	FGoFishingCameraInPositionSignature OnCameraInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingCancelledSignature OnFishingCancelledDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingPlayerInPositionSignature OnPlayerInPositionDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingRodEquippedSignature OnFishingRodEquippedDelegate;
	UPROPERTY(BlueprintAssignable)
	FGoFishingRodCastSignature OnFishingRodCastDelegate;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FGoFishingFishBiteSignature OnFishingBiteDelegate;

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
};
