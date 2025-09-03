// Copyright Alien Shores


#include "Player/PlayerInventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Game/Save/AuraSaveGame.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"

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
			return Entry.ItemType == ItemType && Entry.ItemCount > 0;
		}
	);
}

int32 UPlayerInventoryComponent::AddToInventory(const FGameplayTag& ItemType, const int32 Count)
{
	const FAuraItemDefinition ItemDefinition = UAuraGameDataSubsystem::Get(GetOwner())->FindItemDefinitionByItemTag(ItemType);
	FAuraItemInventoryEntry* ItemEntry = Inventory.FindByPredicate(
		[ItemType](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType.MatchesTagExact(ItemType);
		}
	);
	const bool AddToInventory = !ItemEntry;
	if (!ItemEntry)
	{
		if (Inventory.Num() + ItemDefinition.InventorySize <= MaxItems)
		{
			ItemEntry = new FAuraItemInventoryEntry();
			ItemEntry->ItemType = ItemType;
			ItemEntry->ItemCount = 0;
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
	if (AddToInventory)
	{
		Inventory.Add(*ItemEntry);
	}
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
	Inventory = SaveData->SavedInventory.Inventory;
}

void UPlayerInventoryComponent::ToSaveData(UAuraSaveGame* SaveData) const
{
	SaveData->SavedInventory.MaxItems = MaxItems;
	SaveData->SavedInventory.Inventory = Inventory;
}

TArray<FAuraItemInventoryEntry> UPlayerInventoryComponent::GetInventory() const
{
	return Inventory;
}

bool UPlayerInventoryComponent::UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory)
{
	const FAuraItemDefinition ItemDefinition = UAuraGameDataSubsystem::Get(GetOwner())->FindItemDefinitionByItemTag(ItemTag);
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
			return Entry.ItemType.MatchesTagExact(ItemTag);
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
		UE_LOG(LogAura, Warning, TEXT("[%s][%s] Used item %s"), *GetOwner()->GetName(), *GetName(), *ItemTag.ToString())
		Inventory.RemoveAll([ItemTag](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType.MatchesTagExact(ItemTag);
		});
	}
	OnInventoryItemCountChangedDelegate.Broadcast(FOnInventoryItemCountChangedPayload(
		ItemEntry->ItemType,
		OldValue,
		NewValue
	));
	return true;
}
