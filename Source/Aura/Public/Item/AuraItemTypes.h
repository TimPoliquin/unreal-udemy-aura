#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AuraItemTypes.generated.h"


class AAuraEquipmentBase;

UENUM(BlueprintType)
enum class EAuraItemCategory : uint8
{
	/** Default type - these items have no use**/
	None,
	/** Consumable type - these items can be used 1x **/
	Consumable,
	/** Equipment type - these items can be equipped into equipment slots */
	Equipment,
	/** Fish type - these items are fish **/
	Fish,
	/** Instant type - these items have an effect immediately on contact with the player */
	Instant,
	/** Used to unlock doors/chests/etc */
	Key,
};

UENUM(BlueprintType)
enum class EAuraEquipmentCategory : uint8
{
	/** Default equipment type - this probably indicates a configuration error */
	None,
	/** Tool equipment type - these items can be equipped and used to perform special tasks */
	Tool,
	/** Weapon equipment type - these items can be equipped and allow the player to perform in combat */
	Weapon,
};

UENUM(BlueprintType)
enum class EAuraEquipmentSlot : uint8
{
	/** None */
	None,
	/** Tool slot */
	Tool,
	/** Weapon slot */
	Weapon
};

UENUM(BlueprintType)
enum class EAuraEquipmentUseMode : uint8
{
	/** The player is using nothing */
	None,
	/** The player is using a tool */
	Tool,
	/** The player is using a weapon */
	Weapon
};

USTRUCT(BlueprintType)
struct FAuraItemDefinition
{
	GENERATED_BODY()


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FString ItemName = FString("INVALID");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FString ItemDescription = FString("");
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Item.Type"))
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	EAuraItemCategory ItemCategory = EAuraItemCategory::None;
	/** Determines the maximum number of this item that can exist in the inventory **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	int32 InventoryMaxCount = 1;
	/** Determines how many slots the item occupies in the inventory **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	int32 InventorySize = 1;
	/** If the item is equipment, this specifies which type of equipment it is */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item|Equipment")
	EAuraEquipmentCategory EquipmentCategory = EAuraEquipmentCategory::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(Categories="Message"))
	FGameplayTag PickupMessageTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TSubclassOf<AAuraEquipmentBase> ItemClass;

	bool IsValid() const
	{
		return ItemType.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FAuraItemInventoryEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Item", meta=(Categories="Item.Type"))
	FGameplayTag ItemType = FGameplayTag::EmptyTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory|Item")
	int32 ItemCount = 0;

	bool operator==(const FAuraItemInventoryEntry& Other) const
	{
		return ItemType.MatchesTagExact(Other.ItemType);
	}

	bool IsValid() const
	{
		return ItemType.IsValid();
	}
};

USTRUCT(BlueprintType)
struct FMessageSubstitutions
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<FString, FString> Substitutions;

	void Add(const FString& Key, const FString& Value)
	{
		Substitutions.Add(Key, Value);
	}
};
