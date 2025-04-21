// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Components/ActorComponent.h"
#include "Interaction/FishingComponentInterface.h"
#include "Item/AuraFishingBob.h"
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
	EAuraItemType WeaponType = EAuraItemType::None;
	EAuraItemType ToolType = EAuraItemType::None;

	void Reset()
	{
		bSet = false;
		UseMode = EAuraEquipmentUseMode::None;
		WeaponType = EAuraItemType::None;
		ToolType = EAuraItemType::None;
	}
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
	virtual void FishStateChanged(const EFishState& FishState) override;
	virtual void Reel() override;
	virtual FOnFishingStateChangedSignature& GetOnFishingStateChangedDelegate() override;
	virtual void PrepareForContinue() override;
	UFUNCTION(BlueprintCallable)
	virtual void EndFishing() override;
	UFUNCTION(BlueprintCallable)
	void ReleaseCast();
	virtual bool IsFishing() const override;
	UFUNCTION(BlueprintCallable)
	EFishingState GetFishingState() const;
	UFUNCTION(BlueprintCallable)
	virtual float GetRarityMultiplier(const EFishRarity& Rarity) const override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual TArray<EFishTag> GetFishingTags() const override;

	UPROPERTY(BlueprintAssignable)
	FOnFishingStateChangedSignature OnFishingStateChangedDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnFishingComponentCastAnimationSignature OnFishingComponentCastAnimationDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFishingState FishingState = EFishingState::None;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<EFishTag> FishingTags;

private:
	void SetFishingState(EFishingState InFishingState);
	void OnFishingRodEquipped();
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
	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveReelAbilityHandle;
};
