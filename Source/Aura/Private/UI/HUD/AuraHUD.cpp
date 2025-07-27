// Copyright Alien Shores


#include "UI/HUD/AuraHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/Widget/AuraMenuWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();
	InitializeInventoryViewModel();
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
	AActor* InPlayer,
	APlayerController* InPlayerController,
	APlayerState* InPlayerState,
	UAbilitySystemComponent* InAbilitySystemComponent,
	UAttributeSet* InAttributeSet
)
{
	OverlayWidget = CreateAuraWidget(
		OverlayWidgetClass,
		OverlayWidgetControllerClass,
		InPlayer,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	MenuWidget = CreateWidget<UAuraMenuWidget>(GetWorld(), MenuWidgetClass, FName("MenuWidget"));
	MenuWidget->InitializeDependencies(
		InPlayer,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	MenuWidget->AddToViewport();
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

UMVVM_Inventory* AAuraHUD::GetInventoryViewModel()
{
	return InventoryViewModel;
}

UAuraUserWidget* AAuraHUD::CreateAuraWidget(
	TSubclassOf<UAuraUserWidget> WidgetClass,
	TSubclassOf<UAuraWidgetController> WidgetControllerClass,
	AActor* InOwner,
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
		InOwner,
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

void AAuraHUD::InitializeInventoryViewModel()
{
	InventoryViewModel = NewObject<UMVVM_Inventory>(this, InventoryViewModelClass);
	InventoryViewModel->InitializeInventoryItems();
	InventoryViewModel->InitializeDependencies(GetOwner());
}
