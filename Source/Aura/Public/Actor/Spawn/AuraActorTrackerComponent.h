// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "AuraActorTrackerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraActorTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAuraActorTrackerComponent();

	UFUNCTION(BlueprintCallable)
	void Track(AActor* Actor);
	UPROPERTY(BlueprintAssignable)
	FOnAuraActorTrackerCountChangedDelegate OnCountChanged;

protected:
	UPROPERTY(VisibleInstanceOnly)
	TArray<AActor*> Actors;

private:
	UFUNCTION()
	void OnTrackedActorDestroyed(AActor* DestroyedActor);
};
