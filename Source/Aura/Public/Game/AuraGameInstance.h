// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UAuraCharacterGameInstanceSubsystem;
class ULocalPlayerSaveGameSubsystem;
class ULevelGameInstanceSubsystem;
/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	static UAuraGameInstance* Get(const UObject* WorldContextObject);
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActivePlayerActors;

	virtual void Shutdown() override;

	void RegisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Add(Actor);
	}

	void UnregisterActivePlayer(AActor* Actor)
	{
		ActivePlayerActors.Remove(Actor);
	}

	TArray<AActor*> GetActivePlayerActors();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<UAuraCharacterGameInstanceSubsystem> CharacterSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<ULevelGameInstanceSubsystem> LevelSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<ULocalPlayerSaveGameSubsystem> SaveGameSubsystem;
};
