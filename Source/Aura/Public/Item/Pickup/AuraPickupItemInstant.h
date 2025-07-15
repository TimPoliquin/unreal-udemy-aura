// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupItemBase.h"
#include "AuraPickupItemInstant.generated.h"

UCLASS()
class AURA_API AAuraPickupItemInstant : public AAuraPickupItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraPickupItemInstant();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UAuraEffectComponent> EffectComponent;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
