﻿// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/TimelineComponent.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCameraComponent.generated.h"


class UTimelineComponent;

struct FCameraAnimationParams
{
	FVector StartDirection = FVector::ZeroVector;
	FVector StartLocation = FVector::ZeroVector;
	FVector EndDirection = FVector::ZeroVector;
	FVector EndLocation = FVector::ZeroVector;
	FOnCameraMoveFinishedSignature* Callback = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UAuraCameraComponent();

	void MoveToLocation(
		const FVector& Destination,
		const FVector& Direction,
		UCurveFloat* AnimationCurve,
		FOnCameraMoveFinishedSignature* Callback = nullptr
	);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UTimelineComponent> CameraTimeline;

	FOnTimelineFloat OnTimelineTickDelegate;
	FOnTimelineEvent OnTimelineFinishedDelegate;

	UFUNCTION()
	void OnTimelineTick(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	FCameraAnimationParams AnimationParams;
};
