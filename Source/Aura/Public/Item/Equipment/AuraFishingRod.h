// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraEquipmentBase.h"
#include "Item/Equipment/AuraFishingBob.h"
#include "AuraFishingRod.generated.h"

class UCableComponent;
class AAuraFishingBob;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingBobLandedSignature);

UCLASS()
class AURA_API AAuraFishingRod : public AAuraEquipmentBase
{
	GENERATED_BODY()

public:
	AAuraFishingRod();

	UFUNCTION(BlueprintCallable)
	FVector GetRodTipLocation() const;
	UFUNCTION(BlueprintCallable)
	FRotator GetRodTipRotation() const;
	UFUNCTION(BlueprintCallable)
	void Cast(const FVector& Destination);
	void Return();
	UPROPERTY(BlueprintAssignable)
	FOnFishingBobLandedSignature OnFishingBobLandedDelegate;
	virtual void UnEquip(AActor* InOwner) override;
	AAuraFishingBob* GetFishingBob() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Fishing Rod")
	TSubclassOf<AAuraFishingBob> FishingBobClass;
	UPROPERTY(BlueprintReadOnly, Category = "Fishing Rod")
	TObjectPtr<AAuraFishingBob> BobActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fishing Rod")
	TObjectPtr<UCableComponent> CableComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fishing Rod")
	FName RodTipSocket = FName(TEXT("RodTip"));


	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnFishingBobStateChanged(EFishingBobState FishingState);
	AAuraFishingBob* CreateFishingBob();
};
