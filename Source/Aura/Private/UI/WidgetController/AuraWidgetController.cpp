// Copyright Alien Shores


#include "UI/WidgetController/AuraWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Player/AuraPlayerState.h"
#include "Tags/AuraGameplayTags.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastAbilityInfo()
{
	if (!GetAuraAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		return;
	}
	if (!AbilityInfo)
	{
		UE_LOG(LogAura, Error, TEXT("Ability Info needs to be set in Widget Controller [%s]"), *GetName());
	}
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			// Get ability tag for given ability spec.
			const FGameplayTag AbilityTag = UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec);
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.InputTag = UAuraAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			Info.StatusTag = UAuraAbilitySystemLibrary::GetStatusTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	GetAuraAbilitySystemComponent()->ForEachAbility(BroadcastDelegate);
}

void UAuraWidgetController::UnbindAll_Implementation(const UObject* BoundObject)
{
	AbilityInfoDelegate.RemoveAll(BoundObject);
}

AAuraPlayerController* UAuraWidgetController::GetAuraPlayerController()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AAuraPlayerController>(PlayerController);
	}
	return AuraPlayerController;
}

AAuraPlayerState* UAuraWidgetController::GetAuraPlayerState()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = Cast<AAuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UAuraAbilitySystemComponent* UAuraWidgetController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UAuraAttributeSet* UAuraWidgetController::GetAuraAttributeSet()
{
	{
		if (AuraAttributeSet == nullptr)
		{
			AuraAttributeSet = Cast<UAuraAttributeSet>(AttributeSet);
		}
		return AuraAttributeSet;
	}
}
