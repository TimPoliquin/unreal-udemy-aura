﻿// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraFishingBob.generated.h"

class UProjectileMovementComponent;
class USphereComponent;


UENUM(BlueprintType)
enum class EFishingBobState : uint8
{
	None,
	Casting,
	Bobbing,
	Reeling
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFishingBobStateChangedSignature, EFishingBobState, FishingBobState);


UCLASS()
class AURA_API AAuraFishingBob : public AActor
{
	GENERATED_BODY()

public:
	AAuraFishingBob();
	virtual void Tick(float DeltaTime) override;
	FName GetMeshComponentName() const { return FName("Mesh"); }
	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& InDestination);

	UPROPERTY(BlueprintAssignable)
	FOnFishingBobStateChangedSignature OnFishingStateChanged;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float DestinationThreshold = 10.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FishingBob")
	EFishingBobState FishingState = EFishingBobState::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float BobbingPeriodMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="FishingBob")
	float BobbingAmplitude = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FishingBob|Debug")
	bool bDrawDebug = false;

private:
	FVector Destination;
	float BobbingTime = 0.f;

	void HandleCastingTick(float DeltaTime);
	void HandleBobbingTick(float DeltaTime);
	void HandleReelingTick(float DeltaTime);

	void SetFishingState(const EFishingBobState InFishingState)
	{
		FishingState = InFishingState;
		OnFishingStateChanged.Broadcast(InFishingState);
	}
};
