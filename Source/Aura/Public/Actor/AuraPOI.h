// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraEffectActor.h"
#include "AuraInteractionInterface.h"
#include "GameFramework/Actor.h"
#include "AuraPOI.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class AURA_API AAuraPOI : public AAuraEffectActor, public IAuraInteractionInterface
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="POI")
	TObjectPtr<UWidgetComponent> PreconditionWidget;
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
	UFUNCTION(BlueprintNativeEvent)
	bool IsPreconditionMet(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowInteractWithPOIAvailable(AActor* Player) const;
	UFUNCTION(BlueprintNativeEvent)
	void ShowPreconditionWidget(AActor* Player) const;

	/** AuraInteractionInterface Start **/
	virtual bool OnInteract_Implementation(AActor* Player) override;
	UFUNCTION(BlueprintImplementableEvent)
	bool HandleInteract(AActor* Player);
	virtual void OnInteractionEnd_Implementation(AActor* Player, const bool bIsCancelled) override;
	/** AuraInteractionInterface End **/

private:
	bool IsPlayerActor(const AActor* Actor) const;
};
