// Copyright Alien Shores


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraInventoryComponent.h"
#include "Player/Progression/AuraProgressionComponent.h"
#include "Tags/AuraGameplayTags.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Vital_Health, GetAuraAttributeSet()->GetHealth()));
	OnMaxMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Secondary_MaxHealth, GetAuraAttributeSet()->GetMaxHealth()));
	OnManaChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Vital_Mana, GetAuraAttributeSet()->GetMana()));
	OnMaxManaChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Secondary_MaxMana, GetAuraAttributeSet()->GetMaxMana()));
	if (const UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		const float NewPercentage = UAuraGameDataSubsystem::Get(GetAuraPlayerState())->GetXPToNextLevelPercentage(ProgressionComponent->GetXP());
		OnXPPercentageChanged.Broadcast(FAuraFloatAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Progression_XP, NewPercentage));
		OnPlayerLevelChangedDelegate.Broadcast(FAuraIntAttributeChangedPayload::CreateBroadcastPayload(AuraGameplayTags.Attributes_Progression_Level, ProgressionComponent->GetCharacterLevel()));
	}
	if (GetAuraAbilitySystemComponent()->HasFiredOnAbilitiesGivenDelegate())
	{
		BroadcastAbilityInfo();
	}
	else
	{
		GetAuraAbilitySystemComponent()->OnAbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
	}
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	if (UAuraProgressionComponent* ProgressionComponent = UAuraProgressionComponent::Get(GetAuraPlayerState()))
	{
		ProgressionComponent->OnXPChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerXPChange);
		ProgressionComponent->OnLevelInitializedDelegate.AddDynamic(
			this,
			&UOverlayWidgetController::OnPlayerLevelInitialized
		);
		ProgressionComponent->OnLevelChangeDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerLevelChange);
	}
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetHealthAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      OnHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Health, Data.OldValue, Data.NewValue));
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxHealthAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxMaxHealthChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Secondary_MaxHealth, Data.OldValue, Data.NewValue));
		                      }
	                      );

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetManaAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      OnManaChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Vital_Mana, Data.OldValue, Data.NewValue));
		                      }
	                      );
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAttributeSet()->GetMaxManaAttribute())
	                      .AddLambda(
		                      [&](const FOnAttributeChangeData& Data)
		                      {
			                      OnMaxManaChanged.Broadcast(FAuraFloatAttributeChangedPayload(GameplayTags.Attributes_Secondary_MaxMana, Data.OldValue, Data.NewValue));
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
	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Player_HUD_Hide,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &UOverlayWidgetController::OnPlayerHideHUDTagChanged);
	if (UAuraInventoryComponent* PlayerInventoryComponent = UAuraInventoryComponent::Get(GetAuraPlayerState()))
	{
		PlayerInventoryComponent->OnInventoryItemCountChangedDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryChanged);
		PlayerInventoryComponent->OnInventoryFullDelegate.AddDynamic(this, &UOverlayWidgetController::OnPlayerInventoryFull);
	}
}

bool UOverlayWidgetController::IsBroadcastPayload(const FAuraIntAttributeChangedPayload& Payload)
{
	return !Payload.IsChanged();
}

void UOverlayWidgetController::OnPlayerXPChange(const FAuraIntAttributeChangedPayload& Payload)
{
	const float OldPercentage = UAuraGameDataSubsystem::Get(GetAuraPlayerState())->GetXPToNextLevelPercentage(Payload.OldValue);
	const float NewPercentage = UAuraGameDataSubsystem::Get(GetAuraPlayerState())->GetXPToNextLevelPercentage(Payload.NewValue);
	OnXPPercentageChanged.Broadcast(FAuraFloatAttributeChangedPayload(FAuraGameplayTags::Get().Attributes_Progression_XP, OldPercentage, NewPercentage));
}

void UOverlayWidgetController::OnPlayerLevelInitialized(const FAuraIntAttributeChangedPayload& Payload)
{
	OnPlayerLevelInitializedDelegate.Broadcast(Payload);
}

void UOverlayWidgetController::OnPlayerLevelChange(const FAuraIntAttributeChangedPayload& Payload)
{
	OnPlayerLevelChangedDelegate.Broadcast(Payload);
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

void UOverlayWidgetController::OnPlayerInventoryChanged(const FOnInventoryItemCountChangedPayload& Payload)
{
	UAuraGameDataSubsystem* GameDataSubsystem = UAuraGameDataSubsystem::Get(Player);
	if (!GameDataSubsystem)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s] Could not get game data subsystem for Player [%s]"), *GetName(), *Player->GetName())
		return;
	}
	const FAuraItemDefinition ItemDefinition = GameDataSubsystem->FindItemDefinitionByItemTag(Payload.ItemType);
	FGameplayTag MessageTag;
	if (!ItemDefinition.IsValid())
	{
		// do nothing - invalid item type
		return;
	}
	if (Payload.IsItemAddedChange())
	{
		MessageTag = ItemDefinition.PickupMessageTag.IsValid() ? ItemDefinition.PickupMessageTag : GameDataSubsystem->GetDefaultItemPickupMessageTag();
	}
	else if (Payload.IsItemUsedChange())
	{
		MessageTag = ItemDefinition.UseMessageTag.IsValid() ? ItemDefinition.UseMessageTag : GameDataSubsystem->GetDefaultItemUseMessageTag();
	}
	if (!MessageTag.IsValid())
	{
		return;
	}
	if (const FUIWidgetRow* WidgetRow = GetDataTableRowByTag<FUIWidgetRow>(MessageDataTable, MessageTag))
	{
		FMessageSubstitutions Substitutions;
		Substitutions.Add(FString("ItemName"), ItemDefinition.ItemName);
		Substitutions.Add(FString("Count"), FString::FromInt(FMath::Abs(Payload.GetDelta())));
		Substitutions.Icon = ItemDefinition.ItemIcon;
		MessageWidgetRowDelegate.Broadcast(*WidgetRow, Substitutions);
	}
}

void UOverlayWidgetController::OnPlayerInventoryFull(const FGameplayTag& ItemType)
{
}
