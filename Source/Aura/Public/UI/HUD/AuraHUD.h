// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraWidgetController;
class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UAuraUserWidget;
class UOverlayWidgetController;
class APlayerController;
class APlayerState;
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams);
	void InitializeWidgets(
		APlayerController* InPlayerController,
		APlayerState* InPlayerState,
		UAbilitySystemComponent* InAbilitySystemComponent,
		UAttributeSet* InAttributeSet
	);

	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(
		const FWidgetControllerParams& WidgetControllerParams
	);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UAuraUserWidget> OverlayWidget;

private:
	/** Overlay Widget Controller **/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	/** Attribute Menu Controller **/
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAuraUserWidget> AttributeMenuWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UAuraUserWidget* CreateWidget(
		TSubclassOf<UAuraUserWidget> WidgetClass,
		TSubclassOf<UAuraWidgetController> WidgetControllerClass,
		APlayerController* InPlayerController,
		APlayerState* InPlayerState,
		UAbilitySystemComponent* InAbilitySystemComponent,
		UAttributeSet* InAttributeSet
	);
};
