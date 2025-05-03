// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UFishInfo;
class UAuraItemInfo;
class ULootTiers;
class UAuraSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
class UAuraGameInstance;
/**
 * 
 */
UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	FORCEINLINE UCharacterClassInfo* GetCharacterClassInfo() const
	{
		return CharacterClassInfo;
	}

	FORCEINLINE UAbilityInfo* GetAbilityInfo() const
	{
		return AbilityInfo;
	}

	FORCEINLINE UAuraItemInfo* GetItemInfo() const
	{
		return ItemInfo;
	}

	FORCEINLINE UFishInfo* GetFishInfo() const
	{
		return FishInfo;
	}

	UFUNCTION()
	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot);
	void SaveInGameProgressData(UAuraSaveGame* SaveGame);
	UAuraSaveGame* GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const;
	void LoadMap(const UMVVM_LoadSlot* LoadSlot);
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	static void DeleteSlot(const FString& SlotName, const int32 SlotIndex);
	FString GetDefaultMapName() const;
	FString GetDefaultMapAssetName() const;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	UAuraGameInstance* GetAuraGameInstance() const;
	FName GetDefaultPlayerStartTag() const;
	void SetDefaultPlayerStartTag(const FName& InDefaultPlayerStartTag);
	UAuraSaveGame* GetInGameSaveData() const;
	int32 GetDefaultPlayerLevel() const;
	void SetDefaultPlayerLevel(const int32 InDefaultPlayerLevel);
	void PlayerDied(ACharacter* PlayerCharacter);
	static AAuraGameModeBase* GetAuraGameMode(const UObject* WorldContextObject);
	ULootTiers* GetLootTiers() const { return LootTiers; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category="Items")
	TObjectPtr<UAuraItemInfo> ItemInfo;

	UPROPERTY(EditDefaultsOnly, Category="Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	UPROPERTY(EditDefaultsOnly, Category="Fishing")
	TObjectPtr<UFishInfo> FishInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	int32 DefaultPlayerLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly, Category="Maps")
	FString DefaultMapName;
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	TSoftObjectPtr<UWorld> DefaultMap;
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	FName DefaultPlayerStartTag;
	UPROPERTY(EditDefaultsOnly, Category="Maps")
	TMap<FString, TSoftObjectPtr<UWorld>> MapsByName;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
};
