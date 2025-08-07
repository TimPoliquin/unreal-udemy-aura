// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "Beacon.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeaconStatusChangedSignature, const ABeacon*, Beacon);

UENUM(BlueprintType)
enum class EBeaconValidationState : uint8
{
	Incomplete,
	OutOfOrder,
	Complete
};

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
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnBeaconStatusChangedSignature OnBeaconActivated;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnBeaconStatusChangedSignature OnBeaconReset;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static EBeaconValidationState ValidateBeaconActivation(const ABeacon* CurrentBeacon, const TArray<ABeacon*>& Beacons, bool IsOrdered);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckpointReached(UMaterialInstanceDynamic* DynamicInstance);

	UFUNCTION(BlueprintCallable)
	void ActivateBeacon();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ResetBeacon();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsBeaconActivated() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> BeaconMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bHasBeenActivated = false;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> OriginalMaterial;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;
};
