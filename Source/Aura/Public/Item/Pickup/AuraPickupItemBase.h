// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interaction/HighlightInterface.h"
#include "AuraPickupItemBase.generated.h"

class USphereComponent;
class UAuraEffectComponent;
class USinusoidalMovementComponent;

UCLASS()
class AURA_API AAuraPickupItemBase : public AActor, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAuraPickupItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> SphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USinusoidalMovementComponent> SinusoidalMovementComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<USoundBase> PickupSound;


	/** IHighlightInterface Start **/
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	/** IHighlightInterface End **/

	UFUNCTION(BlueprintNativeEvent)
	void PlayPickupEffect(AActor* PickupActor, const bool bAutoDestroy);
};
