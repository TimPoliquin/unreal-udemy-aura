// Copyright Alien Shores


#include "Item/Effect/AuraSpawnEffectActor.h"


// Sets default values
AAuraSpawnEffectActor::AAuraSpawnEffectActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AAuraSpawnEffectActor::BeginPlay()
{
	Super::BeginPlay();
}


void AAuraSpawnEffectActor::PlaySpawnEffect_Implementation(const FAuraSpawnEffectParams& SpawnParams)
{
	// nothing to do here right now.
}

FTransform AAuraSpawnEffectActor::CalculateTransform(const FAuraSpawnEffectParams& SpawnParams, const float Alpha, const float HeightAlpha, const float ScaleAlpha)
{
	FTransform NewTransform;
	const FVector AlphaLocation = FMath::Lerp(SpawnParams.SpawnTransform.GetLocation(), SpawnParams.TargetTransform.GetLocation(), Alpha);
	const FVector AdjustedLocation = AlphaLocation + FVector(0, 0, SpawnParams.HeightMultiplier * HeightAlpha);
	NewTransform.SetLocation(AdjustedLocation);
	NewTransform.SetScale3D(FVector(ScaleAlpha, ScaleAlpha, ScaleAlpha));
	return NewTransform;
}
