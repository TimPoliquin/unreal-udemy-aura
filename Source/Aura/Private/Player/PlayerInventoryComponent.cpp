// Copyright Alien Shores


#include "Player/PlayerInventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraSaveGame.h"
#include "GameFramework/Character.h"
#include "Item/Equipment/AuraFishingRod.h"
#include "Item/Equipment/AuraEquipmentBase.h"
#include "Item/AuraItemBlueprintLibrary.h"
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
	const FAuraItemDefinition ItemDefinition = AAuraGameModeBase::GetAuraGameMode(GetOwner())->FindItemDefinitionByItemTag(ItemType);
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
			Inventory.Add(*ItemEntry);
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
	const int32 OldValue = ItemEntry->ItemCount;
	const int32 NewValue = ItemEntry->ItemCount + CountToAdd;
	ItemEntry->ItemCount = NewValue;
	OnInventoryItemCountChangedDelegate.Broadcast(FOnInventoryItemCountChangedPayload(
		ItemEntry->ItemType,
		OldValue,
		NewValue
	));
	return CountToAdd;
}

bool UPlayerInventoryComponent::UseConsumable(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EAuraItemCategory::Consumable);
}

bool UPlayerInventoryComponent::UseKey(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EAuraItemCategory::Key);
}

void UPlayerInventoryComponent::FromSaveData(const UAuraSaveGame* SaveData)
{
	MaxItems = SaveData->SavedInventory.MaxItems;
	EquipmentUseMode = SaveData->SavedInventory.EquipmentUseMode;
	Inventory = SaveData->SavedInventory.Inventory;
	EquipmentSlots = SaveData->SavedInventory.EquipmentSlots;
}

void UPlayerInventoryComponent::ToSaveData(UAuraSaveGame* SaveData) const
{
	SaveData->SavedInventory.MaxItems = MaxItems;
	SaveData->SavedInventory.EquipmentUseMode = EquipmentUseMode;
	SaveData->SavedInventory.Inventory = Inventory;
	SaveData->SavedInventory.EquipmentSlots = EquipmentSlots;
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


AAuraEquipmentBase* UPlayerInventoryComponent::SpawnEquipment(const EAuraEquipmentSlot& Slot)
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
	AAuraEquipmentBase* Equipment = GetWorld()->SpawnActor<AAuraEquipmentBase>(
		ItemDefinition.ItemClass,
		SocketLocation,
		SocketRotation,
		SpawnParameters
	);
	if (!Equipment)
	{
		UE_LOG(LogAura, Error, TEXT("[%s][%s] Failed to spawn equipment for slot [%s]. Check item configuration."), *GetOwner()->GetName(), *GetName(), *ItemDefinition.ItemType.ToString());
		return nullptr;
	}
	Equipment->AttachToComponent(
		Player->GetMesh(),
		FAttachmentTransformRules::KeepWorldTransform,
		SocketName
	);
	return Equipment;
}

bool UPlayerInventoryComponent::UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory)
{
	const FAuraItemDefinition ItemDefinition = AAuraGameModeBase::GetAuraGameMode(GetOwner())->FindItemDefinitionByItemTag(ItemTag);
	if (!ItemDefinition.IsValid())
	{
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] Attempted to find item that has no definition: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	if (ItemDefinition.ItemCategory != ItemCategory)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] Attempted to use item of incorrect category: %s != %s"), *GetOwner()->GetName(), *GetName(), *UEnum::GetValueAsString(ItemCategory),
		       *UEnum::GetValueAsString(ItemDefinition.ItemCategory));
		return false;
	}
	FAuraItemInventoryEntry* ItemEntry = Inventory.FindByPredicate(
		[ItemTag](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType == ItemTag;
		}
	);
	if (!ItemEntry || !ItemEntry->IsValid())
	{
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] Item not found in inventory: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	if (ItemEntry->ItemCount <= 0)
	{
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] Item found in inventory but with 0 count: %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString());
		return false;
	}
	const int32 OldValue = ItemEntry->ItemCount;
	const int32 NewValue = ItemEntry->ItemCount - 1;
	ItemEntry->ItemCount = NewValue;
	if (ItemEntry->ItemCount <= 0)
	{
		Inventory.Remove(*ItemEntry);
	}
	OnInventoryItemCountChangedDelegate.Broadcast(FOnInventoryItemCountChangedPayload(
		ItemEntry->ItemType,
		OldValue,
		NewValue
	));
	return true;
}
