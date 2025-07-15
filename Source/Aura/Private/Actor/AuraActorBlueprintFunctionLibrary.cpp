// Copyright Alien Shores


#include "Actor/AuraActorBlueprintFunctionLibrary.h"

void UAuraActorBlueprintFunctionLibrary::SinusoidalMovement(
	AActor* Actor,
	const float DeltaSeconds,
	float& RunningTime,
	const float PeriodMultiplier,
	const float Amplitude,
	const FVector& InitialLocation
)
{
	const float SinePeriod = 2 * PI / PeriodMultiplier;
	RunningTime += DeltaSeconds;
	if (RunningTime > SinePeriod)
	{
		RunningTime -= SinePeriod;
	}
	const float Sine = Amplitude * FMath::Sin(RunningTime * PeriodMultiplier);
	Actor->SetActorLocation(InitialLocation + FVector(0.f, 0.f, Sine));
}

void UAuraActorBlueprintFunctionLibrary::DisableActor(AActor* Actor)
{
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
}
