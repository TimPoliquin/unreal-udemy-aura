// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraPickupItemBase.h"
#include "TieredItemInterface.h"
#include "AuraPickupItemInstant.generated.h"

UCLASS()
class AURA_API AAuraPickupItemInstant : public AAuraPickupItemBase, public ITieredItemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraPickupItemInstant();
	/** Start ITieredItemInterface **/
	virtual int32 GetItemLevel_Implementation() const override;
	virtual void SetItemLevel_Implementation(int32 Level) override;
	/** End ITieredItemInterface **/

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
