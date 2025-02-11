// Copyright Alien Shores


#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
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
	OverlayWidget = CreateWidget(
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
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(
			this,
			AttributeMenuWidgetControllerClass
		);
		AttributeMenuWidgetController->SetWidgetControllerParams(WidgetControllerParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

UAuraUserWidget* AAuraHUD::CreateWidget(
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
