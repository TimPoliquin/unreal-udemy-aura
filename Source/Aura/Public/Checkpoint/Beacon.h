// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "Beacon.generated.h"

class USphereComponent;

UCLASS()
class AURA_API ABeacon : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	ABeacon();
	/**
	 * Save Interface
	 */
	FORCEINLINE virtual bool ShouldLoadTransform_Implementation() const override { return false; }
	virtual void LoadActor_Implementation() override;
	/**
	 *Save Interface End
	 */

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicInstance);

	UFUNCTION(BlueprintCallable)
	void HandleGlowEffects();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BeaconMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	bool bHasBeenActivated = false;
};
