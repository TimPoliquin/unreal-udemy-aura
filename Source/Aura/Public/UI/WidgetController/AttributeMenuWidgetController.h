// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraWidgetController.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;
struct FAuraAttributeInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnAttributePointsChangedDelegate;

	UFUNCTION(BlueprintCallable, Category = "GAS|Attributes")
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GAS|Attributes")
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
	UFUNCTION()
	void OnAttributePointsChanged(int32 InPoints);
};
