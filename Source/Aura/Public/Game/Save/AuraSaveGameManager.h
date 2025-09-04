// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "AuraSaveGameTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AuraSaveGameManager.generated.h"

class UAuraSaveGame;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraSaveEventSignature, const bool, bSuccess);

USTRUCT(BlueprintType)
struct AURA_API FAuraSaveGameParams
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite)
	FString DestinationMapName = FString("");
	UPROPERTY(BlueprintReadWrite)
	FName DestinationPlayerStartTag = NAME_None;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class AURA_API UAuraSaveGameManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UAuraSaveGameManager* Get(const UObject* WorldContextObject);
	UAuraSaveGameManager();

	// Subsystem overrides
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// Save/Load functions
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void AutoSave_LevelTransition(const FAuraSaveGameParams& SaveParams);
	UFUNCTION(BlueprintCallable, Category = "Save System")
	void AutoLoad_LevelTransition();

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void SaveGame(const FAuraSaveGameParams& SaveParams);

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void LoadGame(const FString& SlotName = "Default");

	UFUNCTION(BlueprintCallable, Category = "Save System")
	bool DoesSaveGameExist(const FString& SlotName = "Default") const;

	UFUNCTION(BlueprintCallable, Category = "Save System")
	void DeleteSaveGame(const FString& SlotName = "Default");

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Save System")
	FAuraSaveEventSignature OnSaveCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Save System")
	FAuraSaveEventSignature OnLoadCompleted;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAuraSaveGame> SaveGameClass;
	UPROPERTY(EditDefaultsOnly)
	bool bAllowDefaultSave = false;

	UAuraSaveGame* GetCurrentPrimarySaveGame() const;
	UAuraSaveGame* GetCurrentAutoSaveGame() const;
	// Internal save/load logic
	void SaveMetaData(UAuraSaveGame* SaveGame);
	void SaveGlobalData(UAuraSaveGame* SaveGame);
	void SaveWorldData(UAuraSaveGame* SaveData, FWorldSaveData& WorldSaveData);
	void SaveActorsData(UAuraSaveGame* SaveData);
	void SaveActorData(AActor* Actor, FActorSaveData& ActorData);
	void SaveComponentData(UActorComponent* Component, FComponentSaveData& ComponentData);
	void SaveGameData(UAuraSaveGame* CurrentSaveData);

	void LoadMetaData(const UAuraSaveGame* SaveData);
	void LoadGlobalData(const UAuraSaveGame* SaveData);
	void LoadWorldData(const UAuraSaveGame* SaveData, const FWorldSaveData& WorldSaveData);
	void LoadActorsData(const UAuraSaveGame* SaveData, const FWorldSaveData& WorldSaveData);
	void LoadActor(AActor* TargetActor, const FActorSaveData& ActorData);
	AActor* LoadActorData(const FActorSaveData& ActorData);
	void LoadComponentData(UActorComponent* Component, const FComponentSaveData& ComponentData);

private:
	// Current save data
	FString CurrentSaveSlotName;
	bool bIsMostRecentSaveAutoSave = false;
	// Utility functions
	FString GetAutoSaveName() const;
	UFUNCTION()
	void OnLevelLoadComplete(UWorld* World);
};
