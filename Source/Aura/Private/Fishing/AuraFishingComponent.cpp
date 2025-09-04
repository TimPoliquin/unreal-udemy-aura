// Copyright Alien Shores


#include "Fishing/AuraFishingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Fishing/AuraFishInfo.h"
#include "Game/AuraGameState.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"
#include "Item/Equipment/AuraFishingRod.h"
#include "Player/AuraPlayerEquipmentComponent.h"
#include "Player/AuraInventoryComponent.h"
#include "Tags/AuraGameplayTags.h"


UAuraFishingComponent::UAuraFishingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAuraFishingComponent::SetPlayerEquipmentComponent(UAuraPlayerEquipmentComponent* InPlayerEquipmentComponent)
{
	PlayerEquipmentComponent = InPlayerEquipmentComponent;
}

void UAuraFishingComponent::SetupForFishing(const FVector& InFishingDestination)
{
	FishingDestination = InFishingDestination;
	if (IsValid(PlayerEquipmentComponent))
	{
		PlayerEquipmentComponent->OnEquipmentAnimationCompleteDelegate.AddUniqueDynamic(
			this,
			&UAuraFishingComponent::OnInventoryEquipAnimationComplete
		);
	}
	else
	{
		UE_LOG(LogAura, Warning, TEXT("[%s:%s] No player inventory component set! Unable to bind equipment animation completion delegate."), *GetOwner()->GetName(), *GetName())
	}
	if (!FishingRestore.bSet)
	{
		FishingRestore.UseMode = PlayerEquipmentComponent->GetEquipmentUseMode();
		FishingRestore.WeaponType = PlayerEquipmentComponent->GetWeaponType();
		FishingRestore.ToolType = PlayerEquipmentComponent->GetToolType();
		FishingRestore.bSet = true;
	}
}

bool UAuraFishingComponent::HasFishingRod() const
{
	if (const UAuraInventoryComponent* PlayerInventoryComponent = UAuraInventoryComponent::Get(GetOwner()))
	{
		return PlayerInventoryComponent->HasItemInInventory(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod);
	}
	UE_LOG(LogAura, Warning, TEXT("[%s:%s] No player inventory component!"), *GetOwner()->GetName(), *GetName())
	return false;
}

bool UAuraFishingComponent::HasFishingRodEquipped() const
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s:%s] No player inventory component set!"), *GetOwner()->GetName(), *GetName())
		return false;
	}
	return PlayerEquipmentComponent->HasToolEquipped(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod);
}

void UAuraFishingComponent::EquipFishingRod()
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogAura, Error, TEXT("[%s:%s] No PlayerEquipmentComponent set!"), *GetOwner()->GetName(), *GetName())
		return;
	}
	if (PlayerEquipmentComponent->HasToolEquipped(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		OnFishingRodEquipped();
	}
	else
	{
		PlayerEquipmentComponent->Equip(
			EAuraEquipmentSlot::Tool,
			FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod
		);
		PlayerEquipmentComponent->PlayEquipAnimation(EAuraEquipmentSlot::Tool);
	}
}

void UAuraFishingComponent::CastFishingRod()
{
	if (!PlayerEquipmentComponent)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s:%s] No player inventory component set!"), *GetOwner()->GetName(), *GetName())
		return;
	}
	SetFishingState(EFishingState::Casting);
	OnFishingComponentCastAnimationDelegate.Broadcast();
}

void UAuraFishingComponent::FishStateChanged(const EFishState& FishState)
{
	switch (FishState)
	{
	case EFishState::None:

		break;
	case EFishState::Lured:
		SetFishingState(EFishingState::Lured);
		FishingBob->Lured();
		break;
	case EFishState::Biting:
		SetFishingState(EFishingState::Biting);
		FishingBob->Biting();
		break;
	case EFishState::Fighting:
		// DEVNOTE - nothing to do here - yet.
		break;
	case EFishState::Caught:
		SetFishingState(EFishingState::Caught);
		if (FishingRod)
		{
			FishingRod->Return();
		}
		if (FishingBob)
		{
			FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
		}
		break;
	case EFishState::Fled:
		SetFishingState(EFishingState::Fled);
		if (FishingRod)
		{
			FishingRod->Return();
		}
		if (FishingBob)
		{
			FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
		}
		break;
	}
}

void UAuraFishingComponent::Reel()
{
	SetFishingState(EFishingState::Reeling);
}

FOnFishingStateChangedSignature& UAuraFishingComponent::GetOnFishingStateChangedDelegate()
{
	return OnFishingStateChangedDelegate;
}

void UAuraFishingComponent::PrepareForContinue()
{
	SetFishingState(EFishingState::Preparing);
	OnFishingStateChangedDelegate.Clear();
}

void UAuraFishingComponent::EndFishing()
{
	if (FishingState == EFishingState::None)
	{
		UE_LOG(LogAura, Warning, TEXT("Fishing has already ended"));
		return;
	}
	SetFishingState(EFishingState::None);
	FishingDestination = FVector::ZeroVector;
	OnFishingStateChangedDelegate.Clear();
	if (FishingRod)
	{
		// TODO - clear fishing rod data
		FishingRod = nullptr;
	}
	if (FishingBob)
	{
		FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
		FishingBob->Cancel();
		FishingBob = nullptr;
	}
	switch (FishingRestore.UseMode)
	{
	case EAuraEquipmentUseMode::None:
		PlayerEquipmentComponent->PlayEquipAnimation(EAuraEquipmentSlot::None);
		break;
	case EAuraEquipmentUseMode::Tool:
		PlayerEquipmentComponent->Equip(EAuraEquipmentSlot::Tool, FishingRestore.ToolType);
		PlayerEquipmentComponent->PlayEquipAnimation(EAuraEquipmentSlot::Tool);
		break;
	case EAuraEquipmentUseMode::Weapon:
		PlayerEquipmentComponent->Equip(EAuraEquipmentSlot::Weapon, FishingRestore.WeaponType);
		PlayerEquipmentComponent->PlayEquipAnimation(EAuraEquipmentSlot::Weapon);
	}
	FishingRestore.Reset();
}

void UAuraFishingComponent::ReleaseCast()
{
	if (FishingRod)
	{
		SetFishingState(EFishingState::Casting);
		FishingRod->Cast(FishingDestination);
	}
	else
	{
		UE_LOG(
			LogAura,
			Warning,
			TEXT("UAuraFishingComponent::CastFishingRod - Attempted to release cast before rod has been equipped")
		);
	}
}

bool UAuraFishingComponent::IsFishing() const
{
	return FishingState > EFishingState::None;
}

EFishingState UAuraFishingComponent::GetFishingState() const
{
	return FishingState;
}

float UAuraFishingComponent::GetRarityMultiplier(const FGameplayTag& Rarity) const
{
	const int32 FishingLevel = UAuraAbilitySystemLibrary::GetAbilityLevelByAbilityTag(
		GetOwner(),
		FAuraGameplayTags::Get().Abilities_Fishing
	);
	return UAuraGameDataSubsystem::Get(GetOwner())->GetFishInfo()->
	                                                GetFishRarityMultiplierByPlayerFishingLevel(
		                                                FishingLevel,
		                                                Rarity
	                                                );
}

FGameplayTagContainer UAuraFishingComponent::GetFishingTags() const
{
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		FGameplayTagContainer FishingTags;
		FishingTags.AppendMatchingTags(
			AbilitySystemComponent->GetOwnedGameplayTags(),
			FAuraGameplayTags::Get().Fish_Tag.GetSingleTagContainer()
		);
		return FishingTags;
	}
	return FGameplayTagContainer();
}

void UAuraFishingComponent::SetFishingState(EFishingState InFishingState)
{
	if (InFishingState != FishingState)
	{
		FishingState = InFishingState;
		OnFishingStateChangedDelegate.Broadcast(InFishingState);
	}
	else
	{
		UE_LOG(LogAura, Warning, TEXT("AuraFishingComponent: Potential loop detected setting state"));
	}
}

void UAuraFishingComponent::OnFishingRodEquipped()
{
	SetFishingState(EFishingState::Equipped);
	FishingRod = PlayerEquipmentComponent->GetFishingRod();
	if (IsValid(FishingRod))
	{
		FishingBob = FishingRod->GetFishingBob();
	}
	if (IsValid(FishingBob))
	{
		FishingBob->OnFishingStateChanged.AddDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
	}
	PlayerEquipmentComponent->OnEquipmentAnimationCompleteDelegate.RemoveDynamic(
		this,
		&UAuraFishingComponent::OnInventoryEquipAnimationComplete
	);
}

void UAuraFishingComponent::OnInventoryEquipAnimationComplete(const FAuraEquipmentDelegatePayload& Payload)
{
	if (Payload.EquipmentSlot == EAuraEquipmentSlot::Tool && Payload.EquipmentTag.MatchesTagExact(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		OnFishingRodEquipped();
	}
}

void UAuraFishingComponent::OnFishingBobStateChanged(EFishingBobState FishingBobState)
{
	switch (FishingBobState)
	{
	case EFishingBobState::Bobbing:
		SetFishingState(EFishingState::Cast);
		break;
	default:
		break;
	}
}
