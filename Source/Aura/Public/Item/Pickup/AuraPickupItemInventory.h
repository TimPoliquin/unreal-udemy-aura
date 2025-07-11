// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupItemBase.h"
#include "AuraPickupItemInventory.generated.h"

UCLASS()
class AURA_API AAuraPickupItemInventory : public AAuraPickupItemBase
{
	GENERATED_BODY()

public:
	AAuraPickupItemInventory();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item", meta=(ExposeOnSpawn))
	int32 Count = 1;
	UFUNCTION(BlueprintImplementableEvent, Category="Item")
	void OnPickupAvailable(AActor* PickupActor);
	UFUNCTION(BlueprintImplementableEvent, Category="Item")
	void OnPickupUnavailable(AActor* PickupActor);
	UFUNCTION(BlueprintCallable, Category="Item")
	void ActivatePickup(AActor* PickupActor);

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
