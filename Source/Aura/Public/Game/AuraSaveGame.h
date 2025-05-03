// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Item/AuraItemTypes.h"
#include "AuraSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

UENUM(BlueprintType)
enum ESaveSlotAttributeSource
{
	FromDefault,
	FromDisk
};

UENUM(BlueprintType)
enum ESavedAbilityState
{
	GiveAbility,
	GiveAbilityAndActivate
};

USTRUCT(BlueprintType)
struct FSavedActor
{
	GENERATED_BODY()
	UPROPERTY()
	FName ActorName = NAME_None;
	UPROPERTY()
	FTransform Transform = FTransform::Identity;
	// Serialized variables from the actor - as marked with SaveGame specifier
	UPROPERTY()
	TArray<uint8> Bytes;

	bool operator==(const FSavedActor& Other) const
	{
		return ActorName.IsEqual(Other.ActorName);
	}
};

USTRUCT(BlueprintType)
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName = FString();
	UPROPERTY()
	TArray<FSavedActor> SavedActors;

	bool operator==(const FSavedMap& Other) const
	{
		return MapAssetName.Equals(Other.MapAssetName);
	}
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityTypeTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilityStatusTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FGameplayTag AbilitySlotTag = FGameplayTag::EmptyTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 AbilityLevel = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TEnumAsByte<ESavedAbilityState> AbilityState = GiveAbility;

	FString ToString() const
	{
		return FString::Printf(
			TEXT(
				"Ability Tag: [%s] \n"
				"Ability Type: [%s] \n"
				"Ability Status: [%s] \n"
				"Ability Slot: [%s] \n"
			),
			*AbilityTag.ToString(),
			*AbilityTypeTag.ToString(),
			*AbilityStatusTag.ToString(),
			*AbilitySlotTag.ToString()
		);
	}

	bool operator==(const FSavedAbility& Right) const
	{
		return AbilityTag.MatchesTagExact(Right.AbilityTag);
	}
};

USTRUCT(BlueprintType)
struct FSavedInventory
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxItems = 16;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EAuraEquipmentUseMode EquipmentUseMode = EAuraEquipmentUseMode::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FAuraItemInventoryEntry> Inventory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<EAuraEquipmentSlot, FGameplayTag> EquipmentSlots;
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default_Name");
	UPROPERTY()
	FString MapName = FString("Default_Map_Name");
	UPROPERTY()
	FString MapAssetName = FString("Default_Map_Asset_Name");
	UPROPERTY()
	FName PlayerStartTag = FName("Default_Player_Start");
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;
	UPROPERTY()
	TEnumAsByte<ESaveSlotAttributeSource> SaveSlotAttributeSource = FromDefault;

	/* Player Data */
	UPROPERTY()
	int32 PlayerLevel = 0;
	UPROPERTY()
	int32 PlayerXP = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	UPROPERTY()
	int32 SpellPoints = 0;
	/* Player Attributes */
	UPROPERTY()
	float Strength = 0.f;
	UPROPERTY()
	float Intelligence = 0.f;
	UPROPERTY()
	float Resilience = 0.f;
	UPROPERTY()
	float Vigor = 0.f;

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
	UPROPERTY()
	FSavedInventory SavedInventory;
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	void AddSavedAbility(const FSavedAbility& SaveAbility);
	void AddSavedMap(const FSavedMap& SaveMap);

	FSavedMap GetSavedMapByMapName(const FString& InMapAssetName);
	bool HasMap(const FString& InMapAssetName);
	void ReplaceSavedMap(const FString& InMapAssetName, const FSavedMap& SavedMap);
};
