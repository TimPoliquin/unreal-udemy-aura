// Copyright Alien Shores

#pragma once


#include "CoreMinimal.h"
#include "AuraWidgetController.h"
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
	int32 GetSpellPoints();

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnPlayerStatChangedSignature OnSpellMenuSpellPointsChangedDelegate;

private:
	UFUNCTION()
	void OnSpellPointsChanged(const int32 SpellPoints);
};
