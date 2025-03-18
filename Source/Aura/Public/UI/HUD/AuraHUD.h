// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraHUD.generated.h"

class USpellMenuWidgetController;
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
	USpellMenuWidgetController* GetSpellMenuWidgetController(
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
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UAuraUserWidget* CreateAuraWidget(
		TSubclassOf<UAuraUserWidget> WidgetClass,
		TSubclassOf<UAuraWidgetController> WidgetControllerClass,
		APlayerController* InPlayerController,
		APlayerState* InPlayerState,
		UAbilitySystemComponent* InAbilitySystemComponent,
		UAttributeSet* InAttributeSet
	);

	/** Spell Menu Widget Controller */
	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	template <typename T>
	T* InitializeWidgetController(
		const TSubclassOf<T>& WidgetControllerClass,
		const FWidgetControllerParams& WidgetControllerParams
	);
};

template <typename T>
T* AAuraHUD::InitializeWidgetController(
	const TSubclassOf<T>& WidgetControllerClass,
	const FWidgetControllerParams& WidgetControllerParams
)
{
	T* WidgetController = Cast<T>(
		NewObject<UAuraWidgetController>(
			this,
			WidgetControllerClass
		)
	);
	WidgetController->SetWidgetControllerParams(WidgetControllerParams);
	WidgetController->BindCallbacksToDependencies();
	return WidgetController;
}
