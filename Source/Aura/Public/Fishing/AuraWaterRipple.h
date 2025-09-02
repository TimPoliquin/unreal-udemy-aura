// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraWaterRipple.generated.h"

UCLASS(Blueprintable)
class AURA_API AAuraWaterRipple : public AActor
{
	GENERATED_BODY()

public:
	AAuraWaterRipple();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayRipple(const bool bLoop);
	UFUNCTION(BlueprintImplementableEvent)
	void StopRipple();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStartRippling = false;
};
