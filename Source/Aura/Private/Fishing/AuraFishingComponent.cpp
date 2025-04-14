// Copyright Alien Shores


#include "Fishing/AuraFishingComponent.h"

#include "Aura/AuraLogChannels.h"
#include "Item/AuraFishingRod.h"
#include "Item/AuraItemTypes.h"
#include "Player/PlayerInventoryComponent.h"


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
	FishingRestore.UseMode = PlayerInventoryComponent->GetEquipmentUseMode();
	FishingRestore.WeaponType = PlayerInventoryComponent->GetWeaponType();
	FishingRestore.ToolType = PlayerInventoryComponent->GetToolType();
}

bool UAuraFishingComponent::HasFishingRod_Implementation()
{
	return PlayerInventoryComponent->HasItemInInventory(EAuraItemType::FishingRod);
}

bool UAuraFishingComponent::HasFishingRodEquipped_Implementation()
{
	return PlayerInventoryComponent->HasToolEquipped(EAuraItemType::FishingRod);
}

void UAuraFishingComponent::EquipFishingRod_Implementation()
{
	PlayerInventoryComponent->Equip(EAuraEquipmentSlot::Tool, EAuraItemType::FishingRod);
	PlayerInventoryComponent->PlayEquipAnimation(EAuraEquipmentSlot::Tool);
}

void UAuraFishingComponent::CastFishingRod_Implementation()
{
	SetFishingState(EFishingState::Casting);
	OnFishingComponentCastAnimationDelegate.Broadcast();
}

FOnFishingStateChangedSignature& UAuraFishingComponent::GetOnFishingStateChangedDelegate()
{
	return OnFishingStateChangedDelegate;
}

void UAuraFishingComponent::EndFishing()
{
	FishingDestination = FVector::ZeroVector;
	OnFishingStateChangedDelegate.Clear();
	OnFishingComponentCastAnimationDelegate.Clear();
	if (FishingRod)
	{
		// TODO - clear fishing rod data
		FishingRod = nullptr;
	}
	if (FishingBob)
	{
		FishingBob->OnFishingStateChanged.RemoveDynamic(this, &UAuraFishingComponent::OnFishingBobStateChanged);
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
	SetFishingState(EFishingState::None);
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

void UAuraFishingComponent::SetFishingState(EFishingState InFishingState)
{
	FishingState = InFishingState;
	OnFishingStateChangedDelegate.Broadcast(InFishingState);
}

void UAuraFishingComponent::OnInventoryEquipAnimationComplete(
	EAuraEquipmentSlot EquipmentSlot,
	EAuraItemType EquippedItem
)
{
	if (EquipmentSlot == EAuraEquipmentSlot::Tool && EquippedItem == EAuraItemType::FishingRod)
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
