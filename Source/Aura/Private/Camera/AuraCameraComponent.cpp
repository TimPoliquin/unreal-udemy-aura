// Copyright Alien Shores


#include "Camera/AuraCameraComponent.h"

UAuraCameraComponent::UAuraCameraComponent()
{
	CameraTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("CameraTimeline"));
}

void UAuraCameraComponent::MoveToLocation(
	const FVector& Destination,
	const FVector& Direction,
	UCurveFloat* AnimationCurve,
	FOnCameraMoveFinishedSignature* Callback
)
{
	if (AnimationCurve != nullptr)
	{
		CameraTimeline->AddInterpFloat(AnimationCurve, OnTimelineTickDelegate);
		CameraTimeline->SetTimelineFinishedFunc(OnTimelineFinishedDelegate);
		AnimationParams.StartLocation = GetComponentLocation();
		AnimationParams.StartDirection = GetForwardVector();
		AnimationParams.EndDirection = Direction;
		AnimationParams.EndLocation = Destination;
		AnimationParams.Callback = Callback;
		CameraTimeline->PlayFromStart();
	}
}

void UAuraCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	OnTimelineTickDelegate.BindDynamic(this, &UAuraCameraComponent::OnTimelineTick);
	OnTimelineFinishedDelegate.BindDynamic(this, &UAuraCameraComponent::OnTimelineFinished);
}

void UAuraCameraComponent::OnTimelineTick(float Value)
{
	const FVector NewLocation = FMath::Lerp(AnimationParams.StartLocation, AnimationParams.EndLocation, Value);
	const FVector NewDirection = FMath::Lerp(AnimationParams.StartDirection, AnimationParams.EndDirection, Value);
	SetWorldLocation(NewLocation);
	SetWorldRotation(NewDirection.Rotation());
}

void UAuraCameraComponent::OnTimelineFinished()
{
	SetWorldLocation(AnimationParams.EndLocation);
	if (AnimationParams.Callback)
	{
		AnimationParams.Callback->Broadcast();
	}
	AnimationParams = FCameraAnimationParams();
}
