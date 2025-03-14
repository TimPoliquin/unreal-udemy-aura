// Copyright Alien Shores


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	if (AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState))
	{
		AuraPlayerState->OnXPChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerXPChange);
		AuraPlayerState->OnLevelChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerLevelChange);
	}
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnHealthChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxMaxHealthChanged.Broadcast(Data.NewValue);
		                      }
	                      );

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnManaChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxManaChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	if (UAuraAbilitySystemComponent* AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		AbilitySystemComponent
	))
	{
		AuraAbilitySystemComponent->OnEffectAssetTagsDelegate.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				for (const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, Tag))
						{
							MessageWidgetRowDelegate.Broadcast(*Row);
						}
					}
				}
			}
		);
		if (AuraAbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
		{
			OnInitializeStartupAbilities(AuraAbilitySystemComponent);
		}
		else
		{
			AuraAbilitySystemComponent->OnAbilitiesGivenDelegate.AddUObject(
				this,
				&UOverlayWidgetController::OnInitializeStartupAbilities
			);
		}
	}
}

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* InAbilitySystemComponent)
{
	if (!InAbilitySystemComponent->HasFiredOnAbilitiesGivenDelegate())
	{
		return;
	}
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda(
		[this](const FGameplayAbilitySpec& AbilitySpec)
		{
			// Get ability tag for given ability spec.
			const FGameplayTag AbilityTag = UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(AbilitySpec);
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.InputTag = UAuraAbilitySystemLibrary::GetInputTagFromSpec(AbilitySpec);
			AbilityInfoDelegate.Broadcast(Info);
		}
	);
	InAbilitySystemComponent->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnPlayerXPChange(const int32 XP)
{
	if (const AAuraPlayerState* AuraPlayerState = CastChecked<AAuraPlayerState>(PlayerState))
	{
		OnXPPercentageChanged.Broadcast(AuraPlayerState->GetXPToNextLevelPercentage());
	}
}

void UOverlayWidgetController::OnPlayerLevelChange(const int32 InLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(InLevel);
}
