// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/FishingComponentInterface.h"
#include "Item/AuraFishingBob.h"
#include "Item/AuraItemTypes.h"
#include "AuraFishingComponent.generated.h"


class AAuraFishingBob;
class AAuraFishingRod;
class UPlayerInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentEquipAnimationSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentCastAnimationSignature);

struct FAfterFishingRestore
{
	EAuraEquipmentUseMode UseMode = EAuraEquipmentUseMode::None;
	EAuraItemType WeaponType = EAuraItemType::None;
	EAuraItemType ToolType = EAuraItemType::None;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraFishingComponent : public UActorComponent, public IFishingComponentInterface
{
	GENERATED_BODY()

public:
	UAuraFishingComponent();
	void SetPlayerInventoryComponent(UPlayerInventoryComponent* InPlayerInventoryComponent);
	virtual void SetupForFishing(const FVector& InFishingDestination) override;
	virtual bool HasFishingRod_Implementation() override;
	virtual bool HasFishingRodEquipped_Implementation() override;
	virtual void EquipFishingRod_Implementation() override;
	virtual void CastFishingRod_Implementation() override;
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate() override;
	UFUNCTION(BlueprintCallable)
	virtual void EndFishing() override;
	UFUNCTION(BlueprintCallable)
	void ReleaseCast();
	UFUNCTION(BlueprintCallable)
	bool IsFishing() const;
	UFUNCTION(BlueprintCallable)
	EFishingState GetFishingState() const;

	UPROPERTY(BlueprintAssignable)
	FOnFishingStateChangedSignature OnFishingStateChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnFishingComponentCastAnimationSignature OnFishingComponentCastAnimationDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFishingState FishingState = EFishingState::None;

private:
	void SetFishingState(EFishingState InFishingState);
	UFUNCTION()
	void OnFishingBobStateChanged(EFishingBobState FishingBobState);
	UFUNCTION()
	void OnInventoryEquipAnimationComplete(EAuraEquipmentSlot EquipmentSlot, EAuraItemType EquippedItem);
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	UPROPERTY()
	TObjectPtr<AAuraFishingRod> FishingRod;
	UPROPERTY()
	TObjectPtr<AAuraFishingBob> FishingBob;
	UPROPERTY()
	FVector FishingDestination = FVector::ZeroVector;
	FAfterFishingRestore FishingRestore;
};
