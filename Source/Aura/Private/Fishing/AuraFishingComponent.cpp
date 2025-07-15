// Copyright Alien Shores


#include "Fishing/AuraFishingComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Fishing/AuraFishInfo.h"
#include "Game/AuraGameModeBase.h"
#include "Item/Equipment/AuraFishingRod.h"
#include "Item/AuraItemTypes.h"
#include "Player/PlayerInventoryComponent.h"
#include "Tags/AuraGameplayTags.h"


UAuraFishingComponent::UAuraFishingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAuraFishingComponent::SetPlayerInventoryComponent(UPlayerInventoryComponent* InPlayerInventoryComponent)
{
	PlayerInventoryComponent = InPlayerInventoryComponent;
}

void UAuraFishingComponent::SetupForFishing(const FVector& InFishingDestination)
{
	FishingDestination = InFishingDestination;
	PlayerInventoryComponent->OnEquipmentAnimationCompleteDelegate.AddDynamic(
		this,
		&UAuraFishingComponent::OnInventoryEquipAnimationComplete
	);
	if (!FishingRestore.bSet)
	{
		FishingRestore.UseMode = PlayerInventoryComponent->GetEquipmentUseMode();
		FishingRestore.WeaponType = PlayerInventoryComponent->GetWeaponType();
		FishingRestore.ToolType = PlayerInventoryComponent->GetToolType();
		FishingRestore.bSet = true;
	}
}

bool UAuraFishingComponent::HasFishingRod_Implementation()
{
	return PlayerInventoryComponent->HasItemInInventory(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod);
}

bool UAuraFishingComponent::HasFishingRodEquipped_Implementation()
{
	return PlayerInventoryComponent->HasToolEquipped(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod);
}

void UAuraFishingComponent::EquipFishingRod_Implementation()
{
	if (PlayerInventoryComponent->HasToolEquipped(FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod))
	{
		OnFishingRodEquipped();
	}
	else
	{
		PlayerInventoryComponent->Equip(
			EAuraEquipmentSlot::Tool,
			FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod
		);
		PlayerInventoryComponent->PlayEquipAnimation(EAuraEquipmentSlot::Tool);
	}
}

void UAuraFishingComponent::CastFishingRod_Implementation()
{
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
		PlayerInventoryComponent->PlayEquipAnimation(EAuraEquipmentSlot::None);
		break;
	case EAuraEquipmentUseMode::Tool:
		PlayerInventoryComponent->Equip(EAuraEquipmentSlot::Tool, FishingRestore.ToolType);
		PlayerInventoryComponent->PlayEquipAnimation(EAuraEquipmentSlot::Tool);
		break;
	case EAuraEquipmentUseMode::Weapon:
		PlayerInventoryComponent->Equip(EAuraEquipmentSlot::Weapon, FishingRestore.WeaponType);
		PlayerInventoryComponent->PlayEquipAnimation(EAuraEquipmentSlot::Weapon);
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
	return AAuraGameModeBase::GetAuraGameMode(GetOwner())->GetFishInfo()->
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
		const FGameplayTagContainer& TagContainer = AbilitySystemComponent->GetOwnedGameplayTags();
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
	FishingRod = PlayerInventoryComponent->GetFishingRod();
	if (IsValid(FishingRod))
	{
		FishingBob = FishingRod->GetFishingBob();
	}
	if (IsValid(FishingBob))
	{
		FishingBob->OnFishingStateChanged.AddDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
	}
	PlayerInventoryComponent->OnEquipmentAnimationCompleteDelegate.RemoveDynamic(
		this,
		&UAuraFishingComponent::OnInventoryEquipAnimationComplete
	);
}

void UAuraFishingComponent::OnInventoryEquipAnimationComplete(
	EAuraEquipmentSlot EquipmentSlot,
	const FGameplayTag& EquippedItem
)
{
	if (EquipmentSlot == EAuraEquipmentSlot::Tool && EquippedItem == FAuraGameplayTags::Get().
		Item_Type_Equipment_FishingRod)
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
