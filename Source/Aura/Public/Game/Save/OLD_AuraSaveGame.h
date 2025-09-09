// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSaveGameTypes.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "Item/AuraItemTypes.h"
#include "Player/Equipment/AuraEquipmentTypes.h"
#include "OLD_AuraSaveGame.generated.h"

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

	bool IsValid() const
	{
		return !MapAssetName.IsEmpty();
	}

	bool operator==(const FSavedMap& Other) const
	{
		return MapAssetName.Equals(Other.MapAssetName);
	}

	void FillActorsByName(TMap<FName, FSavedActor>& InActorMap) const
	{
		for (auto Actor : SavedActors)
		{
			InActorMap.Add(Actor.ActorName, Actor);
		}
	}
};


USTRUCT(BlueprintType)
struct FSavedInventory
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxItems = 25;
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
UCLASS(Abstract, Blueprintable)
class AURA_API UOLD_AuraSaveGame : public USaveGame
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
