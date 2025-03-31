// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraProjectile.h"
#include "AuraFireball.generated.h"

UCLASS()
class AURA_API AAuraFireball : public AAuraProjectile
{
	GENERATED_BODY()

public:
	AAuraFireball();

	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsWithinExplodeDistance() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetReturnToLocation() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;


	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float TravelDistance;
	UPROPERTY(EditDefaultsOnly, Category="Fireball")
	float ExplodeDistance = 250.f;
	UPROPERTY(BlueprintReadOnly)
	FVector InitialLocation;
	UPROPERTY(BlueprintReadOnly)
	FVector ApexLocation;
};
