// Copyright Alien Shores


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"
#include "Player/Progression/AuraProgressionComponent.h"
#include "Tags/AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	check(AttributeInfo);
	if (UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		ProgressionComponent->OnAttributePointsChangeDelegate.AddDynamic(
			this,
			&UAttributeMenuWidgetController::OnAttributePointsChanged
		);
	}
	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	check(AttributeInfo);
	for (auto& Pair : GetAuraAttributeSet()->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		OnAttributePointsChanged(FAuraIntAttributeChangedPayload::CreateBroadcastPayload(FAuraGameplayTags::Get().Attributes_Progression_AttributePoints, ProgressionComponent->GetAttributePoints()));
	}
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraAbilitySystemComponent()->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(
	const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute
) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeMenuWidgetController::OnAttributePointsChanged(const FAuraIntAttributeChangedPayload& Payload)
{
	OnAttributePointsChangedDelegate.Broadcast(Payload);
}
