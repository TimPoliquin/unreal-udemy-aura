// Copyright Alien Shores


#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();
}


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WidgetControllerParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitializeWidgets(
	APlayerController* InPlayerController,
	APlayerState* InPlayerState,
	UAbilitySystemComponent* InAbilitySystemComponent,
	UAttributeSet* InAttributeSet
)
{
	OverlayWidget = CreateAuraWidget(
		OverlayWidgetClass,
		OverlayWidgetControllerClass,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams
)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = InitializeWidgetController<UAttributeMenuWidgetController>(
			AttributeMenuWidgetControllerClass,
			WidgetControllerParams
		);
	}
	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(
	const FWidgetControllerParams& WidgetControllerParams
)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = InitializeWidgetController<USpellMenuWidgetController>(
			SpellMenuWidgetControllerClass,
			WidgetControllerParams
		);
	}
	return SpellMenuWidgetController;
}

UAuraUserWidget* AAuraHUD::CreateAuraWidget(
	TSubclassOf<UAuraUserWidget> WidgetClass,
	TSubclassOf<UAuraWidgetController> WidgetControllerClass,
	APlayerController* InPlayerController,
	APlayerState* InPlayerState,
	UAbilitySystemComponent* InAbilitySystemComponent,
	UAttributeSet* InAttributeSet
)
{
	checkf(WidgetClass, TEXT("Widget class uninitialized; please fill out BP_AuraHUD"));
	checkf(
		WidgetControllerClass,
		TEXT("Widget controller class uninitialized; please fill out BP_AuraHUD")
	);

	UAuraUserWidget* Widget = CreateWidget<UAuraUserWidget>(GetWorld(), WidgetClass);

	const FWidgetControllerParams WidgetControllerParams(
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	Widget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
	return Widget;
}
