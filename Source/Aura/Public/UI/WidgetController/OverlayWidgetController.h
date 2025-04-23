// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/AttributeChangeDelegates.h"
#include "Item/AuraItemTypes.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

struct FAuraAbilityInfo;
class UAuraAbilitySystemComponent;
class UAuraUserWidget;
struct FOnAttributeChangeData;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Message"))
	FGameplayTag AssetTag = FGameplayTag();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> MessageWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FMessageWidgetRowSignature,
	FUIWidgetRow,
	Row,
	const FMessageSubstitutions&,
	MessageSubstitutions
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOverlayClearSlotSignature, const FGameplayTag&, SlotTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOverlayVisibilityChangedSignature, const bool, bVisible);


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnXPPercentageChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelInitializedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOverlayClearSlotSignature OnClearSlot;

	UPROPERTY(BlueprintAssignable, Category="HUD")
	FOverlayVisibilityChangedSignature OnHUDVisibilityChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageDataTable;

	template <typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

private:
	UFUNCTION()
	void OnPlayerXPChange(const int32 XP);
	UFUNCTION()
	void OnPlayerLevelInitialized(int32 NewValue);
	UFUNCTION()
	void OnPlayerLevelChange(const int32 InLevel);
	UFUNCTION()
	void OnAbilityEquipped(const FAuraEquipAbilityPayload& EquipPayload);
	UFUNCTION()
	void OnPlayerHideHUDTagChanged(FGameplayTag GameplayTag, int Count);
	UFUNCTION()
	void OnPlayerInventoryAddItem(const FGameplayTag& ItemType, int32 Count, bool BAddedAll);
	UFUNCTION()
	void OnPlayerInventoryFull(const FGameplayTag& ItemType);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
