#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AuraEnemySpawnConfig.generated.h"

class AAuraEnemy;

USTRUCT(BlueprintType)
struct FEnemySpawnConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Config")
	TSubclassOf<AAuraEnemy> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Config")
	int32 EnemyLevel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn Config")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	/** Spawn and fully initialize an enemy. **/
	AAuraEnemy* Spawn(const AActor* SpawnPoint) const;
	/** Spawn and partially initialize an enemy. The caller is responsible for finishing spawning and setting the controller. **/
	AAuraEnemy* SpawnDeferred(const AActor* SpawnPoint) const;
};
