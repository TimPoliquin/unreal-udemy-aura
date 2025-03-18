// Copyright Alien Shores

#pragma once


#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "SpellMenuWidgetController.generated.h"

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
	FGameplayTag GetAbilityStatusTag(const FGameplayTag AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetAvailableSpellPoints();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasAvailableSpellPoints();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanEquipAbility(const FGameplayTag& AbilityTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanPurchaseAbility(const FGameplayTag& AbilityTag);

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnPlayerStatChangedSignature OnSpellMenuSpellPointsChangedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnPlayerStatChangedSignature OnSpellMenuPlayerLevelChangedDelegate;

private:
	UFUNCTION()
	void OnSpellPointsChanged(const int32 SpellPoints);
	UFUNCTION()
	void OnPlayerLevelChanged(const int32 level, const TArray<FAbilityTagStatus> ability_statuses);
};
