// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraMenuWidget.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UAuraInventoryWidget;
/**
 * 
 */
UCLASS()
class AURA_API UAuraMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeDependencies(
		AActor* InPlayer,
		APlayerController* InPlayerController,
		APlayerState* InPlayerState,
		UAbilitySystemComponent* InAbilitySystemComponent,
		UAttributeSet* InAttributeSet
	);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UAuraInventoryWidget> InventoryWidgetClass;
	UPROPERTY()
	TObjectPtr<UAuraInventoryWidget> InventoryWidget;
};
