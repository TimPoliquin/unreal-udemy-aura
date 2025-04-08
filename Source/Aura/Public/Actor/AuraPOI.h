// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraPOI.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class AURA_API AAuraPOI : public AActor
{
	GENERATED_BODY()

public:
	AAuraPOI();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI")
	TObjectPtr<UWidgetComponent> POIWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="POI")
	TObjectPtr<USphereComponent> SphereComponent;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool BFromSweep,
		const FHitResult& SweepResult
	);
	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowInteractionWidget();
	UFUNCTION(BlueprintImplementableEvent)
	void HideInteractionWidget();

private:
	bool IsPlayerActor(const AActor* Actor) const;
};
