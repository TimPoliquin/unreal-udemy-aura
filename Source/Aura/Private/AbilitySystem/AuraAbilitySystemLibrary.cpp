// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WidgetControllerParams;
	GetWidgetControllerParams(WorldContextObject, WidgetControllerParams);
	if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
	{
		return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject
)
{
	FWidgetControllerParams WidgetControllerParams;
	GetWidgetControllerParams(WorldContextObject, WidgetControllerParams);
	if (AAuraHUD* AuraHUD = GetAuraHUD(WorldContextObject))
	{
		return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::GetWidgetControllerParams(
	const UObject* WorldContextObject,
	FWidgetControllerParams& FWidgetControllerParams
)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>();
		UAbilitySystemComponent* AbilitySystemComponent = PlayerState->GetAbilitySystemComponent();
		UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
		FWidgetControllerParams.PlayerController = PlayerController;
		FWidgetControllerParams.PlayerState = PlayerState;
		FWidgetControllerParams.AbilitySystemComponent = AbilitySystemComponent;
		FWidgetControllerParams.AttributeSet = AttributeSet;
	}
}

AAuraHUD* UAuraAbilitySystemLibrary::GetAuraHUD(const UObject* WorldContextObject)
{
	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		return Cast<AAuraHUD>(PlayerController->GetHUD());
	}
	return nullptr;
}
