// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "SpawnEffectInterface.h"
#include "GameFramework/Actor.h"
#include "AuraSpawnEffectActor.generated.h"

UCLASS()
class AURA_API AAuraSpawnEffectActor : public AActor, public ISpawnEffectInterface
{
	GENERATED_BODY()

public:
	AAuraSpawnEffectActor();

protected:
	virtual void BeginPlay() override;
	/** ISpawnEffectInterface Start **/
	virtual void PlaySpawnEffect_Implementation(const FAuraSpawnEffectParams& SpawnParams) override;
	/** ISpawnEffectInterface End **/

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FTransform CalculateTransform(const FAuraSpawnEffectParams& SpawnParams, const float Alpha, const float HeightAlpha, const float ScaleAlpha);

private:
	FTransform SpawnTransform;
};
