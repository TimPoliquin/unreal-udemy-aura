// Copyright Alien Shores

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UAuraAIDirectorGameInstanceSubsystem;
class UAuraGameDataSubsystem;
class USaveGameSubsystem;
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

protected:
	UPROPERTY(EditDefaultsOnly, Category="Subsystem")
	TSubclassOf<UAuraAIDirectorGameInstanceSubsystem> AIDirectorSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<UAuraGameDataSubsystem> CharacterSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<ULevelGameInstanceSubsystem> LevelSubsystem;
	UPROPERTY(EditDefaultsOnly, Category = "Subsystem")
	TSubclassOf<USaveGameSubsystem> SaveGameSubsystem;
};
