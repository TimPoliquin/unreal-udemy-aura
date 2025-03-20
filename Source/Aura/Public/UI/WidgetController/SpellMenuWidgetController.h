﻿// Copyright Alien Shores

#pragma once


#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClearSlotSignature, const FGameplayTag&, SlotTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityAssigned, const FGameplayTag&, AbilityTag);

class FOnPlayerStatChangedSignature;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetAvailableSpellPoints();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAvailableSpellPoints();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanEquipAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanPurchaseAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable)
	void SpendPointOnAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FAuraAbilityDescription GetAbilityDescription(const FGameplayTag AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetAbilityStatusTag(const FGameplayTag AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetAbilityTypeTag(const FGameplayTag& AbilityTag) const;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayTag GetAbilityInputTag(const FGameplayTag AbilityTag);

	UFUNCTION(BlueprintCallable)
	void EquipAbility(
		const FGameplayTag& AbilityTag,
		const FGameplayTag& SlotTag,
		const FGameplayTag& SelectedAbilityTypeTag
	);

	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="BoundObject"))
	void UnbindAll(const UObject* BoundObject);

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnPlayerStatChangedSignature OnSpellMenuSpellPointsChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnPlayerStatChangedSignature OnSpellMenuPlayerLevelChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FClearSlotSignature OnSpellMenuSlotClearedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityAssigned OnAbilityAssigned;

private:
	UFUNCTION()
	void OnSpellPointsChanged(const int32 SpellPoints);
	UFUNCTION()
	void OnPlayerLevelChanged(const int32 Level, const TArray<FAbilityTagStatus>& AbilityStatuses);

	UFUNCTION()
	void OnAbilityEquipped(const FAuraEquipAbilityPayload& EquipPayload);
};
