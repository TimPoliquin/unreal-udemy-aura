// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraFishTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Item/Equipment/AuraFishingBob.h"
#include "Item/AuraItemTypes.h"
#include "AuraFishingComponent.generated.h"


struct FGameplayAbilitySpecHandle;
class AAuraFishingBob;
class AAuraFishingRod;
class UPlayerInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentEquipAnimationSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFishingComponentCastAnimationSignature);

struct FAfterFishingRestore
{
	bool bSet = false;
	EAuraEquipmentUseMode UseMode = EAuraEquipmentUseMode::None;
	FGameplayTag WeaponType = FGameplayTag::EmptyTag;
	FGameplayTag ToolType = FGameplayTag::EmptyTag;

	void Reset()
	{
		bSet = false;
		UseMode = EAuraEquipmentUseMode::None;
		WeaponType = FGameplayTag::EmptyTag;
		ToolType = FGameplayTag::EmptyTag;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AURA_API UAuraFishingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAuraFishingComponent();
	void SetPlayerInventoryComponent(UPlayerInventoryComponent* InPlayerInventoryComponent);
	UFUNCTION(BlueprintCallable)
	virtual void SetupForFishing(const FVector& InFishingDestination);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool HasFishingRod() const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool HasFishingRodEquipped() const;
	UFUNCTION(BlueprintCallable)
	virtual void EquipFishingRod();
	UFUNCTION(BlueprintCallable)
	virtual void CastFishingRod();
	virtual void FishStateChanged(const EFishState& FishState);
	virtual void Reel();
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate();
	virtual void PrepareForContinue();
	UFUNCTION(BlueprintCallable)
	virtual void EndFishing();
	UFUNCTION(BlueprintCallable)
	void ReleaseCast();
	virtual bool IsFishing() const;
	UFUNCTION(BlueprintCallable)
	EFishingState GetFishingState() const;
	UFUNCTION(BlueprintCallable)
	virtual float GetRarityMultiplier(const FGameplayTag& Rarity) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual FGameplayTagContainer GetFishingTags() const;

	UPROPERTY(BlueprintAssignable)
	FOnFishingStateChangedSignature OnFishingStateChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnFishingComponentCastAnimationSignature OnFishingComponentCastAnimationDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFishingState FishingState = EFishingState::None;

private:
	void SetFishingState(EFishingState InFishingState);
	void OnFishingRodEquipped();
	UFUNCTION()
	void OnFishingBobStateChanged(EFishingBobState FishingBobState);
	UFUNCTION()
	void OnInventoryEquipAnimationComplete(EAuraEquipmentSlot EquipmentSlot, const FGameplayTag& EquippedItem);
	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;
	UPROPERTY()
	TObjectPtr<AAuraFishingRod> FishingRod;
	UPROPERTY()
	TObjectPtr<AAuraFishingBob> FishingBob;
	UPROPERTY()
	FVector FishingDestination = FVector::ZeroVector;
	FAfterFishingRestore FishingRestore;
	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveReelAbilityHandle;
};
