// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AuraSpawnEventPayloadDefinitions.h"
#include "AuraActorTrackerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAuraActorTrackerCountChangedDelegate, const FOnAuraActorTrackerCountChangedPayload&, Payload);

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
	UPROPERTY()
	TArray<AActor*> Actors;

private:
	UFUNCTION()
	void OnTrackedActorDestroyed(AActor* DestroyedActor);
};
