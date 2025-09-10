// Copyright Alien Shores


#include "UI/HUD/AuraHUD.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_Inventory.h"
#include "UI/Widget/AuraMenuWidget.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/Widget/AuraOverlayWidget.h"

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
	AActor* InPlayer,
	APlayerController* InPlayerController,
	APlayerState* InPlayerState,
	UAbilitySystemComponent* InAbilitySystemComponent,
	UAttributeSet* InAttributeSet
)
{
	if (bInitialized)
	{
		return;
	}
	OverlayWidget = CreateAuraWidget(
		OverlayWidgetClass,
		OverlayWidgetControllerClass,
		InPlayer,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	OverlayWidget->OnOpenMenuDelegate.AddDynamic(this, &AAuraHUD::OpenMenu);
	InitializeInventoryViewModel();
	MenuWidget = CreateWidget<UAuraMenuWidget>(GetWorld(), MenuWidgetClass, FName("MenuWidget"));
	MenuWidget->InitializeDependencies(
		GetOwningPawn()
	);
	MenuWidget->OnAuraMenuClosed.AddDynamic(this, &AAuraHUD::OnMenuClosed);
	MenuWidget->SetVisibility(ESlateVisibility::Hidden);
	MenuWidget->AddToViewport();
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(InAbilitySystemComponent))
	{
		const FWidgetControllerParams Params = FWidgetControllerParams(InPlayer, InPlayerController, InPlayerState, InAbilitySystemComponent, InAttributeSet);
		if (AuraAbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
		{
			GetOverlayWidgetController(Params)->BroadcastInitialValues();
			GetAttributeMenuWidgetController(Params)->BroadcastInitialValues();
			GetSpellMenuWidgetController(Params)->BroadcastInitialValues();
		}
		else
		{
			AuraAbilitySystemComponent->OnAbilitiesGivenDelegate.AddLambda([this, Params]()
			{
				GetOverlayWidgetController(Params)->BroadcastInitialValues();
				GetAttributeMenuWidgetController(Params)->BroadcastInitialValues();
			});
		}
	}
	bInitialized = true;
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

void AAuraHUD::OpenMenu(const EAuraMenuTab& OpenTab)
{
	GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
	OverlayWidget->Hide();
	MenuWidget->OpenMenu(OpenTab);
}

void AAuraHUD::OnMenuClosed()
{
	OverlayWidget->Show();
	GetOwningPlayerController()->SetInputMode(FInputModeGameAndUI());
}

UAuraOverlayWidget* AAuraHUD::CreateAuraWidget(
	const TSubclassOf<UAuraOverlayWidget>& WidgetClass,
	const TSubclassOf<UAuraWidgetController>& WidgetControllerClass,
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

	UAuraOverlayWidget* Widget = CreateWidget<UAuraOverlayWidget>(GetWorld(), WidgetClass);

	const FWidgetControllerParams WidgetControllerParams(
		InOwner,
		InPlayerController,
		InPlayerState,
		InAbilitySystemComponent,
		InAttributeSet
	);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	Widget->SetWidgetController(WidgetController);
	Widget->AddToViewport();
	return Widget;
}

void AAuraHUD::InitializeInventoryViewModel()
{
	InventoryViewModel = NewObject<UMVVM_Inventory>(this, InventoryViewModelClass);
	InventoryViewModel->InitializeInventoryItems();
	InventoryViewModel->InitializeDependencies();
}
