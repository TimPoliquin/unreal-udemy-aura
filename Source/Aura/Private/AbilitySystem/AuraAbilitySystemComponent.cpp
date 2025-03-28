// Copyright Alien Shores


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAbilitySystemTypes.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Tags/AuraGameplayTags.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"


void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& ForEachAbilityDelegate)
{
	// This locks the ability system abilities for the scope of this function call.
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!ForEachAbilityDelegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in [%hs]"), __FUNCTION__);
		}
	}
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (IPlayerInterface::GetAttributePoints(GetAvatarActor()) > 0)
	{
		ServerUpgradeAttribute(AttributeTag);
	}
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload;
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	IPlayerInterface::SpendAttributePoints(GetAvatarActor(), 1);
}

void UAuraAbilitySystemComponent::ServerUpdateAbilityStatuses(const int32 Level)
{
	const FGameplayTag& EligibleStatusTag = FAuraGameplayTags::Get().Abilities_Status_Eligible;
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	TArray<FAbilityTagStatus> EligibleAbilities;
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if (Level < Info.LevelRequirement || !Info.AbilityTag.IsValid())
		{
			continue;
		}
		if (GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			// TODO - this level is wrong. 
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(EligibleStatusTag);
			GiveAbility(AbilitySpec);
			// Force replication immediately
			MarkAbilitySpecDirty(AbilitySpec);
			// Broadcast to clients
			EligibleAbilities.Add(FAbilityTagStatus::Create(Info.AbilityTag, EligibleStatusTag, AbilitySpec.Level));
		}
	}
	ClientUpdateAbilityStatus(Level, EligibleAbilities);
}

void UAuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(
	const FGameplayTag& AbilityTag,
	bool bActivate
)
{
	OnActivatePassiveEffectDelegate.Broadcast(AbilityTag, bActivate);
}

void UAuraAbilitySystemComponent::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if (IPlayerInterface::GetSpellPoints(GetAvatarActor()) <= 0)
	{
		UE_LOG(LogAura, Warning, TEXT("No spell points to spend points on ability [%s]"), *AbilityTag.ToString())
		return;
	}
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		IPlayerInterface::SpendSpellPoints(GetAvatarActor(), 1);
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		if (const FGameplayTag StatusTag = UAuraAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec);
			StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
		{
			// unlock ability
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || StatusTag.MatchesTagExact(
			GameplayTags.Abilities_Status_Unlocked
		))
		{
			// upgrade ability
			AbilitySpec->Level += 1;
			// TODO Cancel and reactivate the ability if it is passive
		}
		ClientUpdateAbilityStatus(
			ICombatInterface::GetCharacterLevel(GetAvatarActor()),
			FAbilityTagStatus::CreateArray(
				AbilityTag,
				UAuraAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec),
				AbilitySpec->Level
			)
		);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(
	const FGameplayTag& AbilityTag,
	const FGameplayTag& SlotTag
)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		const FGameplayTag PreviousSlot = UAuraAbilitySystemLibrary::GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag Status = UAuraAbilitySystemLibrary::GetStatusTagFromSpec(*AbilitySpec);
		if (UAuraAbilitySystemLibrary::CanEquipAbility(this, AbilityTag))
		{
			if (FGameplayAbilitySpec* SpecWithSlot = GetAbilitySpecWithSlot(SlotTag))
			{
				// there is an ability in this slot already. Deactivate the ability and clear the slot.
				if (AbilityTag.MatchesTagExact(UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot)))
				{
					// equipped the same ability in the same slot - early return.
					ClientEquipAbility(
						FAuraEquipAbilityPayload::Create(
							AbilityTag,
							GameplayTags.Abilities_Status_Equipped,
							SlotTag,
							PreviousSlot
						)
					);
					return;
				}
				if (UAuraAbilitySystemLibrary::IsPassiveAbility(GetAvatarActor(), *SpecWithSlot))
				{
					OnDeactivatePassiveAbilityDelegate.Broadcast(
						UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot)
					);
					MulticastActivatePassiveEffect(
						UAuraAbilitySystemLibrary::GetAbilityTagFromSpec(*SpecWithSlot),
						false
					);
				}
				ClearAbilitySlot(*SpecWithSlot);
			}
			if (!UAuraAbilitySystemLibrary::AbilityHasAnySlot(*AbilitySpec))
			{
				// ability is not yet equipped/active
				if (UAuraAbilitySystemLibrary::IsPassiveAbility(GetAvatarActor(), *AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag, true);
				}
			}
			AssignSlotTagToAbilitySpec(*AbilitySpec, SlotTag);
			MarkAbilitySpecDirty(*AbilitySpec);
			ClientEquipAbility(
				FAuraEquipAbilityPayload::Create(
					AbilityTag,
					GameplayTags.Abilities_Status_Equipped,
					SlotTag,
					PreviousSlot
				)
			);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FAuraEquipAbilityPayload& EquipPayload)
{
	OnAbilityEquippedDelegate.Broadcast(EquipPayload);
}

void UAuraAbilitySystemComponent::ClearAbilitySlot(FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Slot : UAuraAbilitySystemLibrary::GetInputTagsFromSpec(AbilitySpec))
	{
		AbilitySpec.GetDynamicSpecSourceTags().RemoveTag(Slot);
	}
}

void UAuraAbilitySystemComponent::ClearAbilitiesUsingSlot(const FGameplayTag& SlotTag)
{
	FForEachAbility ClearSlotDelegate;
	ClearSlotDelegate.BindLambda(
		[this, SlotTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (UAuraAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag))
			{
				ClearAbilitySlot(AbilitySpec);
			}
		}
	);
	ForEachAbility(ClearSlotDelegate);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability.Get()->GetAssetTags())
		{
			if (Tag.MatchesTagExact(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(
	const FGameplayTag& AbilityTag,
	FAuraAbilityDescription& OutDescription
)
{
	if (!AbilityTag.MatchesTag(FAuraGameplayTags::Get().Abilities))
	{
		return false;
	}
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			OutDescription.Description = AuraAbility->GetDescription(AbilitySpec->Level);
			OutDescription.NextLevelDescription = AuraAbility->GetDescription(AbilitySpec->Level + 1);
			return true;
		}
		UE_LOG(LogAura, Error, TEXT("Ability not set for AbilitySpec assigned to [%s]"), *AbilityTag.ToString())
	}
	const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	OutDescription.Description = UAuraGameplayAbility::GetLockedDescription(
		AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement
	);
	OutDescription.NextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(
		this,
		&UAuraAbilitySystemComponent::Client_EffectApplied
	);
}

bool UAuraAbilitySystemComponent::IsSlotEmpty(const FGameplayTag& SlotTag)
{
	bool HasTagExact = false;
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[SlotTag, &HasTagExact](FGameplayAbilitySpec& AbilitySpec)
		{
			HasTagExact = UAuraAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag);
		}
	);
	ForEachAbility(Delegate);
	return HasTagExact;
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetAbilitySpecWithSlot(const FGameplayTag& SlotTag)
{
	FGameplayAbilitySpec* AbilityInSlot = nullptr;
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[SlotTag, &AbilityInSlot](FGameplayAbilitySpec& AbilitySpec)
		{
			if (UAuraAbilitySystemLibrary::AbilityHasSlotTag(AbilitySpec, SlotTag))
			{
				AbilityInSlot = &AbilitySpec;
			}
		}
	);
	ForEachAbility(Delegate);
	return AbilityInSlot;
}

void UAuraAbilitySystemComponent::AssignSlotTagToAbilitySpec(
	FGameplayAbilitySpec& AbilitySpec,
	const FGameplayTag& SlotTag
)
{
	ClearAbilitySlot(AbilitySpec);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(SlotTag);
	if (SlotTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		// This is a special tag that binds to Shift + Left Click.
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().InputTag_AttackTarget);
	}
}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(
	const int32 PlayerLevel,
	const TArray<FAbilityTagStatus>& AbilityStatuses
)
{
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel, AbilityStatuses);
}

void UAuraAbilitySystemComponent::Client_EffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec,
	FActiveGameplayEffectHandle ActiveEffectHandle
)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	OnEffectAssetTagsDelegate.Broadcast(TagContainer);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities,
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities
)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		FGameplayTag EquippedTag = FAuraGameplayTags::Get().Abilities_Status_Equipped;
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			for (FGameplayTag StartupTag : AuraAbility->GetStartupInputTag())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(StartupTag);
			}
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(EquippedTag);
			GiveAbility(AbilitySpec);
		}
	}
	for (const TSubclassOf PassiveAbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(PassiveAbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
	// NOTE: This is client-side only! OnRep_ActivateAbilities handles server-side.
	bAbilitiesGiven = true;
	OnAbilitiesGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				AbilitySpecInputPressed(AbilitySpec);
				if (AbilitySpec.IsActive())
				{
					InvokeReplicatedEvent(
						EAbilityGenericReplicatedEvent::InputPressed,
						AbilitySpec.Handle,
						UAuraAbilitySystemLibrary::GetPredictionKeyFromAbilitySpec(AbilitySpec)
					);
				}
			}
		}
	);
	ForEachAbility(Delegate);
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;
	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
			{
				AbilitySpecInputPressed(AbilitySpec);
				if (!AbilitySpec.IsActive())
				{
					TryActivateAbility(AbilitySpec.Handle);
				}
			}
		}
	);
	ForEachAbility(Delegate);
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	FForEachAbility Delegate;

	Delegate.BindLambda(
		[this, InputTag](FGameplayAbilitySpec& AbilitySpec)
		{
			if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
				InvokeReplicatedEvent(
					EAbilityGenericReplicatedEvent::InputReleased,
					AbilitySpec.Handle,
					UAuraAbilitySystemLibrary::GetPredictionKeyFromAbilitySpec(AbilitySpec)
				);
			}
		}
	);
	ForEachAbility(Delegate);
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bAbilitiesGiven)
	{
		bAbilitiesGiven = true;
		OnAbilitiesGivenDelegate.Broadcast();
	}
}
