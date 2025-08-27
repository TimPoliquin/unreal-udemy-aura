// Copyright Alien Shores


#include "Actor/AuraActorBlueprintFunctionLibrary.h"

#include "Actor/CollidableInterface.h"
#include "Components/ShapeComponent.h"

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

UShapeComponent* UAuraActorBlueprintFunctionLibrary::FindCollisionComponent(const AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return nullptr;
	}
	if (Actor->Implements<UCollidableInterface>())
	{
		return Cast<ICollidableInterface>(Actor)->GetPrimaryCollisionComponent();
	}
	return Actor->FindComponentByClass<UShapeComponent>();
}

UMeshComponent* UAuraActorBlueprintFunctionLibrary::FindMeshComponent(const AActor* Actor)
{
	return Actor->FindComponentByClass<UMeshComponent>();
}
