// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Game/Save/AuraSaveGame.h"
#include "SaveGameSubsystem.generated.h"

class USaveGame;
class UAuraSaveGame;
class UMVVM_LoadSlot;

UENUM()
enum class EAuraGameSaveState : uint8
{
	Undefined,
	Transient,
	SaveSlot
};

USTRUCT(BlueprintType)
struct FAuraCurrentSaveState
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FName PlayerStartTag = FName();
	UPROPERTY()
	FString SlotName = FString();
	UPROPERTY()
	int32 SlotIndex = 0;
	UPROPERTY()
	EAuraGameSaveState SaveState = EAuraGameSaveState::Undefined;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	UAuraSaveGame* GetInGameSaveData() const;
	UAuraSaveGame* CreateDefaultSaveData(const int32 SlotIndex, const FString& SlotName) const;
	void InitializeSaveState(const UAuraSaveGame* SaveGame, bool bIsTransient);
	void InitializeSaveState(
		const FName& InPlayerStartTag,
		const FString& InSlotName,
		int32 InSlotIndex,
		bool bIsTransient
	);
	FName GetPlayerStartTag() const;

	UFUNCTION()
	void SaveSlotData(UAuraSaveGame* SaveGame) const;
	void SaveInGameProgressData(UAuraSaveGame* SaveGame);
	UAuraSaveGame* GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const;
	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;

	static void DeleteSlot(const FString& SlotName, const int32 SlotIndex);
	static USaveGameSubsystem* Get(const UObject* WorldContextObject);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Save Game")
	bool bAutoSaveOnStart = false;
	UPROPERTY(EditDefaultsOnly, Category="Save Game", meta=(EditCondition="bAutoSaveOnStart", EditConditionHides))
	FString AutoSaveName = FString("Transient");
	UPROPERTY(EditDefaultsOnly, Category="Save Game", meta=(EditCondition="bAutoSaveOnStart", EditConditionHides))
	int32 AutoSaveSlot = 9;
	UPROPERTY(EditDefaultsOnly, Category = "Save Game")
	TSubclassOf<USaveGame> SaveGameClass;

private:
	void AutoSaveTransient();
	UPROPERTY()
	FAuraCurrentSaveState CurrentSaveState = FAuraCurrentSaveState();
};
