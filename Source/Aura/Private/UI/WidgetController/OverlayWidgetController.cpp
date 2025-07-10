// Copyright Alien Shores


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Player/AuraPlayerState.h"
#include "Player/PlayerInventoryComponent.h"
#include "Tags/AuraGameplayTags.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAttributeSet()->GetHealth());
	OnMaxMaxHealthChanged.Broadcast(GetAuraAttributeSet()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAttributeSet()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAttributeSet()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	GetAuraPlayerState()->OnXPChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerXPChange);
	GetAuraPlayerState()->OnLevelInitializedDelegate.AddDynamic(
		this,
		&UOverlayWidgetController::OnPlayerLevelInitialized
	);
	GetAuraPlayerState()->OnLevelChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerLevelChange);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetHealthAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnHealthChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxHealthAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxMaxHealthChanged.Broadcast(Data.NewValue);
		                      }
	                      );

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetManaAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnManaChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxManaAttribute())
	                      .AddLambda(
		                      [this](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxManaChanged.Broadcast(Data.NewValue);
		                      }
	                      );
	GetAuraAbilitySystemComponent()->OnEffectAssetTagsDelegate.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					if (const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, Tag))
					{
						MessageWidgetRowDelegate.Broadcast(*Row, FMessageSubstitutions());
					}
				}
			}
		}
	);
	GetAuraAbilitySystemComponent()->OnAbilityEquippedDelegate.AddDynamic(this, &UOverlayWidgetController::OnAbilityEquipped);
	if (GetAuraAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Player_HUD_Hide,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &UOverlayWidgetController::OnPlayerHideHUDTagChanged);
	if (UPlayerInventoryComponent* PlayerInventoryComponent = UPlayerInventoryComponent::GetPlayerInventoryComponent(Player))
	{
		PlayerInventoryComponent->OnInventoryItemCountChangedDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryAddItem);
		PlayerInventoryComponent->OnInventoryFullDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryFull);
	}
}

void UOverlayWidgetController::OnPlayerXPChange(const int32 XP)
{
	OnXPPercentageChanged.Broadcast(GetAuraPlayerState()->GetXPToNextLevelPercentage());
}

void UOverlayWidgetController::OnPlayerLevelInitialized(int32 NewValue)
{
	OnPlayerLevelInitializedDelegate.Broadcast(NewValue);
}

void UOverlayWidgetController::OnPlayerLevelChange(const int32 InLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(InLevel);
}

void UOverlayWidgetController::OnAbilityEquipped(const FAuraEquipAbilityPayload& EquipPayload)
{
	// clear the previously occupied slot
	OnClearSlot.Broadcast(EquipPayload.PreviousSlotTag);

	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(EquipPayload.AbilityTag);
	Info.StatusTag = EquipPayload.StatusTag;
	Info.InputTag = EquipPayload.SlotTag;
	AbilityInfoDelegate.Broadcast(Info);
}

void UOverlayWidgetController::OnPlayerHideHUDTagChanged(FGameplayTag GameplayTag, int Count)
{
	UE_LOG(LogAura, Warning, TEXT("[%s]: %d"), *GameplayTag.ToString(), Count);
	OnHUDVisibilityChangedDelegate.Broadcast(Count == 0);
}

void UOverlayWidgetController::OnPlayerInventoryAddItem(const FOnInventoryItemCountChangedPayload& Payload
)
{
	const FAuraItemDefinition ItemDefinition = AAuraGameModeBase::GetAuraGameMode(Player)->FindItemDefinitionByItemTag(Payload.ItemType);
	if (const FUIWidgetRow* WidgetRow = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, ItemDefinition.PickupMessageTag))
	{
		FMessageSubstitutions Substitutions;
		Substitutions.Add(FString("ItemName"), ItemDefinition.ItemName);
		Substitutions.Add(FString("Count"), FString::FromInt(Payload.GetDelta()));
		MessageWidgetRowDelegate.Broadcast(*WidgetRow, Substitutions);
	}
}

void UOverlayWidgetController::OnPlayerInventoryFull(const FGameplayTag& ItemType)
{
}
