// Copyright Alien Shores


#include "Player/AuraInventoryComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameState.h"
#include "Game/Save/AuraSaveGameBlueprintFunctionLibrary.h"
#include "Game/Subsystem/AuraGameDataSubsystem.h"

UAuraInventoryComponent* UAuraInventoryComponent::Get(const UObject* InObject)
{
	if (const AAuraGameState* GameState = AAuraGameState::Get(InObject))
	{
		return GameState->GetInventoryComponent();
	}
	return nullptr;
}

UAuraInventoryComponent::UAuraInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SaveID = UAuraSaveGameBlueprintFunctionLibrary::GenerateSaveID(this);
	SetIsReplicatedByDefault(true);
}

TArray<uint8> UAuraInventoryComponent::SaveData_Implementation()
{
	return SerializeComponentData();
}

bool UAuraInventoryComponent::LoadData_Implementation(const TArray<uint8>& Data)
{
	return DeserializeComponentData(Data);
}

bool UAuraInventoryComponent::ShouldSave_Implementation() const
{
	return true;
}

FString UAuraInventoryComponent::GetSaveID_Implementation() const
{
	return SaveID;
}

bool UAuraInventoryComponent::HasItemInInventory(const FGameplayTag& ItemType) const
{
	return Inventory.ContainsByPredicate(
		[ItemType](const FAuraItemInventoryEntry& Entry)
		{
			return Entry.ItemType == ItemType && Entry.ItemCount > 0;
		}
	);
}

int32 UAuraInventoryComponent::AddToInventory(const FGameplayTag& ItemType, const int32 Count)
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
		ItemType,
		OldValue,
		NewValue
	));
	return CountToAdd;
}

bool UAuraInventoryComponent::UseConsumable(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EAuraItemCategory::Consumable);
}

bool UAuraInventoryComponent::UseKey(const FGameplayTag& ItemType)
{
	return UseItem(ItemType, EAuraItemCategory::Key);
}

TArray<FAuraItemInventoryEntry> UAuraInventoryComponent::GetInventory() const
{
	return Inventory;
}

bool UAuraInventoryComponent::UseItem(const FGameplayTag& ItemTag, const EAuraItemCategory& ItemCategory)
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
		ItemTag,
		OldValue,
		NewValue
	));
	return true;
}

TArray<uint8> UAuraInventoryComponent::SerializeComponentData() const
{
	TArray<uint8> Data;
	FMemoryWriter Writer(Data);

	// Create save data struct
	FAuraInventoryComponentSaveData SaveData;
	SaveData.MaxItems = MaxItems;
	SaveData.InventoryItems = Inventory;

	// Serialize the struct
	Writer << SaveData.MaxItems;
	Writer << SaveData.InventoryItems;

	return Data;
}

bool UAuraInventoryComponent::DeserializeComponentData(const TArray<uint8>& Data)
{
	if (Data.Num() == 0)
	{
		return false;
	}

	FMemoryReader Reader(Data);

	try
	{
		Reader << MaxItems;
		Reader << Inventory;

		return true;
	}
	catch (...)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s:%s] Failed to deserialize data"), *GetOwner()->GetName(), *GetName());
		return false;
	}
}
