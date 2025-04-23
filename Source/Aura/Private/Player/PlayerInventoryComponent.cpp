// Copyright Alien Shores


#include "Player/PlayerInventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Game/AuraGameModeBase.h"
#include "GameFramework/Character.h"
#include "Item/AuraFishingRod.h"
#include "Item/AuraItemBase.h"
#include "Item/AuraItemBlueprintLibrary.h"
#include "Item/AuraItemInfo.h"
#include "Tags/AuraGameplayTags.h"


UPlayerInventoryComponent* UPlayerInventoryComponent::GetPlayerInventoryComponent(const AActor* InActor)
{
	if (IsValid(InActor))
	{
		return InActor->FindComponentByClass<UPlayerInventoryComponent>();
	}
	return nullptr;
}

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPlayerInventoryComponent::HasItemInInventory(const FGameplayTag& ItemType) const
{
	return Inventory.ContainsByPredicate(
		[ItemType](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType == ItemType;
		}
	);
}

bool UPlayerInventoryComponent::HasToolEquipped(const FGameplayTag& ItemType) const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Tool && EquipmentSlots[EAuraEquipmentSlot::Tool] == ItemType;
}

bool UPlayerInventoryComponent::IsUsingTool() const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Tool;
}

bool UPlayerInventoryComponent::IsUsingWeapon() const
{
	return EquipmentUseMode == EAuraEquipmentUseMode::Weapon;
}

void UPlayerInventoryComponent::UseTool()
{
	if (IsValid(Weapon))
	{
		Weapon->SetHidden(true);
		Weapon->GetMesh()->SetVisibility(false);
		Weapon->UnEquip(GetOwner());
	}
	EquipmentUseMode = EAuraEquipmentUseMode::Tool;
	if (!IsValid(Tool))
	{
		Tool = SpawnEquipment(EAuraEquipmentSlot::Tool);
	}
	if (Tool)
	{
		Tool->SetHidden(false);
		Tool->GetMesh()->SetVisibility(true);
		Tool->Equip(GetOwner());
	}
	OnUseTool.Broadcast();
}

void UPlayerInventoryComponent::UseWeapon()
{
	if (IsValid(Tool))
	{
		Tool->SetHidden(true);
		Tool->GetMesh()->SetVisibility(false);
		Tool->UnEquip(GetOwner());
	}
	EquipmentUseMode = EAuraEquipmentUseMode::Weapon;
	if (!IsValid(Weapon))
	{
		Weapon = SpawnEquipment(EAuraEquipmentSlot::Weapon);
	}
	if (Weapon)
	{
		Weapon->SetHidden(false);
		Weapon->GetMesh()->SetVisibility(true);
		Weapon->Equip(GetOwner());
	}
	OnUseWeapon.Broadcast();
}

void UPlayerInventoryComponent::UseNothing()
{
	Weapon->SetHidden(true);
	Tool->SetHidden(true);
	EquipmentUseMode = EAuraEquipmentUseMode::None;
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		GetOwner()
	))
	{
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(FAuraGameplayTags::Get().Player_Equipped_Tool);
		AbilitySystemComponent->RemoveReplicatedLooseGameplayTag(FAuraGameplayTags::Get().Player_Equipped_Weapon);
	}
}

void UPlayerInventoryComponent::Equip(const EAuraEquipmentSlot& Slot, const FGameplayTag& ItemType)
{
	EquipmentSlots[Slot] = ItemType;
}

USkeletalMeshComponent* UPlayerInventoryComponent::GetWeapon() const
{
	if (EquipmentUseMode == EAuraEquipmentUseMode::Weapon && EquipmentSlots.Contains(EAuraEquipmentSlot::Weapon) &&
		IsValid(Weapon))
	{
		return Weapon->GetMesh();
	}
	return nullptr;
}

FGameplayTag UPlayerInventoryComponent::GetToolType() const
{
	if (EquipmentSlots.Contains(EAuraEquipmentSlot::Tool))
	{
		return EquipmentSlots[EAuraEquipmentSlot::Tool];
	}
	return FGameplayTag::EmptyTag;
}

FGameplayTag UPlayerInventoryComponent::GetWeaponType() const
{
	if (EquipmentSlots.Contains(EAuraEquipmentSlot::Weapon))
	{
		return EquipmentSlots[EAuraEquipmentSlot::Weapon];
	}
	return FGameplayTag::EmptyTag;
}

AAuraFishingRod* UPlayerInventoryComponent::GetFishingRod() const
{
	if (IsValid(Tool) && Tool->GetItemType() == FAuraGameplayTags::Get().Item_Type_Equipment_FishingRod)
	{
		return Cast<AAuraFishingRod>(Tool);
	}
	return nullptr;
}

FGameplayTag UPlayerInventoryComponent::GetEquippedItem(const EAuraEquipmentSlot Slot) const
{
	if (EquipmentSlots.Contains(Slot))
	{
		return EquipmentSlots[Slot];
	}
	return FGameplayTag::EmptyTag;
}

EAuraEquipmentUseMode UPlayerInventoryComponent::GetEquipmentUseMode() const
{
	return EquipmentUseMode;
}

void UPlayerInventoryComponent::PlayEquipAnimation(const EAuraEquipmentSlot Slot) const
{
	const FGameplayTag& ItemType = GetEquippedItem(Slot);
	OnEquipmentAnimationRequest.Broadcast(Slot, ItemType);
}

int32 UPlayerInventoryComponent::AddToInventory(const FGameplayTag& ItemType, const int32 Count)
{
	const FAuraItemDefinition ItemDefinition = AAuraGameModeBase::GetAuraGameMode(GetOwner())->GetItemInfo()->
		FindItemByItemType(ItemType);
	FAuraItemInventoryEntry* ItemEntry = Inventory.FindByPredicate(
		[ItemType](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType == ItemType;
		}
	);
	if (!ItemEntry)
	{
		if (Inventory.Num() + ItemDefinition.InventorySize <= MaxItems)
		{
			ItemEntry = new FAuraItemInventoryEntry();
			ItemEntry->ItemType = ItemType;
		}
		else
		{
			OnInventoryFullDelegate.Broadcast(ItemType);
			return 0;
		}
	}
	if (ItemEntry->ItemCount >= ItemDefinition.InventoryMaxCount)
	{
		// This item is already at max capacity in the inventory
		OnInventoryFullDelegate.Broadcast(ItemType);
		return 0;
	}
	const int32 CountToAdd = FMath::Min(Count, ItemDefinition.InventoryMaxCount - ItemEntry->ItemCount);
	ItemEntry->ItemCount += CountToAdd;
	OnItemAddedDelegate.Broadcast(ItemType, CountToAdd, CountToAdd == Count);
	return CountToAdd;
}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	switch (EquipmentUseMode)
	{
	case EAuraEquipmentUseMode::Tool:
		UseTool();
		break;
	case EAuraEquipmentUseMode::Weapon:
		UseWeapon();
		break;
	default:
		UseNothing();
		break;
	}
}


AAuraItemBase* UPlayerInventoryComponent::SpawnEquipment(const EAuraEquipmentSlot& Slot)
{
	if (!EquipmentSlots.Contains(Slot))
	{
		return nullptr;
	}
	ACharacter* Player = Cast<ACharacter>(GetOwner());
	const FName SocketName = EquipmentSocketNames[Slot];
	const FVector SocketLocation = Player->GetMesh()->GetSocketLocation(SocketName);
	const FRotator SocketRotation = Player->GetMesh()->GetSocketRotation(SocketName);

	const FAuraItemDefinition ItemDefinition = UAuraItemBlueprintLibrary::GetItemDefinitionByItemType(
		this,
		EquipmentSlots[Slot]
	);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Player;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAuraItemBase* Equipment = GetWorld()->SpawnActor<AAuraItemBase>(
		ItemDefinition.ItemClass,
		SocketLocation,
		SocketRotation,
		SpawnParameters
	);
	Equipment->AttachToComponent(
		Player->GetMesh(),
		FAttachmentTransformRules::KeepWorldTransform,
		SocketName
	);
	return Equipment;
}
